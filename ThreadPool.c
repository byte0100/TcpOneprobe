//
// Created by 54766 on 2019/5/26.
//

#include <time.h>
#include "TCPServer.h"
#include "ThreadPool.h"
#include "taskQueue.h"
#include "MsgFormat.h"

void Init_Thread_pool(ThreadPool* pool, Queue* queue){
    /*
     * 线程池初始化
     * 传入参数：pool 线程池结构指针； queue 任务消息队列
     */

    pthread_mutex_init(&pool->mutex,NULL);
    pthread_cond_init(&pool->cond,NULL);

    pool->task_queue = queue;

    int i = 0;
    for(i = 0; i<MAX_THREADPOOL_SIZE; i++){
        int ret = pthread_create(&pool->pid[i],NULL,pthread_func,(void*)pool);
        if(ret!=0){
            puts("pthread_create() error");
            return;
        }
    }
}

void Add_Task(ThreadPool* pool, Clnt_info clnt_info){
    if(!queue_add(pool->task_queue,clnt_info)){
        printf("taskqueue full!\n");
    }
    pthread_cond_signal(&pool->cond);
}

void* pthread_func(void* arg){
    ThreadPool* pool = (ThreadPool*) arg;
    while(1){
        pthread_mutex_lock(&pool->mutex);
        while(pool->task_queue->size == 0){     //为什么要用循环  线程肯定可以拿到client sock
            pthread_cond_wait(&pool->cond,&pool->mutex);
        }
        Clnt_info clnt_info;
        bzero(&clnt_info,0);

        int ret = queue_pop(pool->task_queue,&clnt_info);
        if(ret != 0){
            puts(" task queue_pop() error");
        }
        pthread_mutex_unlock(&pool->mutex);

        struct timeval tv;
        gettimeofday(&tv,NULL);

        struct tm *time_tm;
        time_tm = localtime(&tv.tv_sec);

        pthread_t tid;
        tid = pthread_self();

        //“ts0, Client IP:Port connected, thread thread_id” log func
        if(clnt_info.log_flag == 1){
             dprintf(clnt_info.log_fd,"%04d-%02d-%02d %02d:%02d:%02d:%03ld:%3ld Client %s:%d connected Thread %ld\n",
                   (time_tm->tm_year + 1900),time_tm->tm_mon+1,time_tm->tm_mday ,time_tm->tm_hour,time_tm->tm_min,
                   time_tm->tm_sec,tv.tv_usec/1000,tv.tv_usec%1000,inet_ntoa(clnt_info.clnt_adr.sin_addr),clnt_info.clnt_adr.sin_port,tid);

        }

        int str_len;
        int dataSize = 0; // 生成的data总大小
        MsgHeader* clnt_msgheader = (MsgHeader*)malloc(sizeof(MsgHeader));

        int msgsize = sizeof(MsgHeader);
//    printf("msgsize = %d\n", msgsize); // 测试msgsize
        char* msgbuffer = (char*)malloc(msgsize);

        char* data;

        while(( recv(clnt_info.fd, msgbuffer, msgsize, 0))){
//            printf("msgbuffer = %s\n", msgbuffer);
//            printf("recv size = %d\n", str_len);
            memset(clnt_msgheader,0,msgsize);
            memcpy(clnt_msgheader, msgbuffer, msgsize);
            printf("recv from client %d messageHeader = %s controlMask = %d dataSize = %d\n", clnt_info.clnt_id, clnt_msgheader->messageHeader,
                   clnt_msgheader->controlMask, clnt_msgheader->dataSize); //检验是否正确解析收到的控制命令
            if(clnt_msgheader->messageHeader[3] == 'O'){
                if(clnt_msgheader->controlMask == CONTROL_INIT){   //Init
                    dataSize = clnt_msgheader->dataSize;
                    data = (char*) malloc(dataSize); // INIT只执行一次 开销不大
                    char replybuf[4] = "ACK";
                    str_len = send(clnt_info.fd, replybuf, sizeof(replybuf),0);
                    Generate_str(data, dataSize); // 生成内容
                    printf("CONTROL_INIT, Thread id = %ld,send = %d\n",tid,str_len ); //检查是否到达CONTROL_INIT状态
                }

                if(clnt_msgheader->controlMask == CONTROL_ACK){
                    send(clnt_info.fd, data, dataSize, 0);
                    printf("CONTROL_ACK, Thread id = %ld,send = %d\n",tid,str_len); //检查是否到达CONTROL_ACK
                }

                if(clnt_msgheader->controlMask == CONTROL_GET){
                    str_len = send(clnt_info.fd, data, dataSize,0);  // 调用一次就能发送 但只是512字节 应该是没超过buffer todo 这里还需要测试大size的data
                    printf("CONTROL_GET, Thread id = %ld,send = %d\n",tid,str_len); //检查是否到达CONTROL_GET
                }
            }
        }
        // todo 退出机制还需要修改 测试 通过设置recv超时 可以退出线程 但主线程不能自己退出
        close(clnt_info.fd);
        gettimeofday(&tv,NULL);
        time_tm = localtime(&tv.tv_sec);
        //"ts1, Client IP:Port disconnected, thread thread_id quit"
        if(clnt_info.log_flag == 1){
             dprintf(clnt_info.log_fd,"%04d-%02d-%02d %02d:%02d:%02d:%03ld:%03ld Client %s:%d disconnected Thread %ld quit\n",
                   (time_tm->tm_year + 1900),time_tm->tm_mon+1,time_tm->tm_mday ,time_tm->tm_hour,time_tm->tm_min,
                   time_tm->tm_sec,tv.tv_usec/1000,tv.tv_usec%1000,inet_ntoa(clnt_info.clnt_adr.sin_addr),clnt_info.clnt_adr.sin_port,tid);

        }
//        printf("thread quit\n");
        return NULL;
    }
}

char* Generate_str(char* pointer, int n){ //生成指定大小的字符串
    int i, index;
    char str[] = "SUSTECH_FNR";
    for(i = 0; i < n; i++){
        index =  i % 11;
        *pointer = str[index];
        pointer++;
    }
    *pointer = '\0';
    return (pointer - n);
}