//
// Created by 54766 on 2019/5/21.
//

#ifndef AGENTV2_THREADPOOL_H
#define AGENTV2_THREADPOOL_H

#define MAX_THREADPOOL_SIZE 1000

typedef struct ThreadPool{
    pthread_t pid[MAX_THREADPOOL_SIZE];     //线程池
    Queue* task_queue;      //任务消息队列

    pthread_mutex_t mutex;      //互斥锁 保护任务消息队列
    pthread_cond_t  cond;       //条件变量 通知子线程 任务消息队列 有新的任务（连接）
}ThreadPool;

void* pthread_func(void* arg);
char* Generate_str(char* pointer, int n);

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

void Add_Task(ThreadPool* pool, int clnt_sock){
    queue_add(pool->task_queue,clnt_sock);
    pthread_cond_signal(&pool->cond);
}


void* pthread_func(void* arg){
    ThreadPool* pool = (ThreadPool*) arg;
    while(1){
        pthread_mutex_lock(&pool->mutex);
        while(pool->task_queue->size == 0){     //为什么要用循环  线程肯定可以拿到client sock
            pthread_cond_wait(&pool->cond,&pool->mutex);
        }
        pthread_t tid;
        tid = pthread_self();
        printf("Thread id = %ld start task\n", tid);
        int clnt_sock = queue_pop(pool->task_queue);
        pthread_mutex_unlock(&pool->mutex);
        int str_len;
        int dataSize = 0; // 生成的data总大小
        MsgHeader* clnt_msgheader = (MsgHeader*)malloc(sizeof(MsgHeader));


        int msgsize = sizeof(MsgHeader);
//    printf("msgsize = %d\n", msgsize); // 测试msgsize
        char* msgbuffer = (char*)malloc(msgsize);

        char* data;
//    data = (char*) malloc(DATA_SIZE); 移动到line 98

        while((str_len = recv(clnt_sock, msgbuffer, msgsize, 0))){
//            printf("msgbuffer = %s\n", msgbuffer);
//            printf("recv size = %d\n", str_len);
            memset(clnt_msgheader,0,msgsize);
            memcpy(clnt_msgheader, msgbuffer, msgsize);
            printf("recv from client %d messageHeader = %s controlMask = %d dataSize = %d\n", clnt_sock, clnt_msgheader->messageHeader,
                   clnt_msgheader->controlMask, clnt_msgheader->dataSize); //检验是否正确解析收到的控制命令
            if(clnt_msgheader->messageHeader[3] == 'O'){
                if(clnt_msgheader->controlMask == CONTROL_INIT){   //Init
                    dataSize = clnt_msgheader->dataSize;
                    data = (char*) malloc(dataSize); // INIT只执行一次 开销不大
                    char replybuf[4] = "ACK";
                    str_len = write(clnt_sock, replybuf, sizeof(replybuf));
                    Generate_str(data, dataSize); // 生成内容
                    printf("CONTROL_INIT, Thread id = %ld,write = %d\n",tid,str_len ); //检查是否到达CONTROL_INIT状态
                }

                if(clnt_msgheader->controlMask == CONTROL_ACK){
                    send(clnt_sock, data, dataSize, 0);
                }

                if(clnt_msgheader->controlMask == CONTROL_GET){
                    str_len = send(clnt_sock, data, dataSize,0);  // 调用一次就能发送 但只是512字节 应该是没超过buffer todo 这里还需要测试大size的data
                    printf("CONTROL_GET, Thread id = %ld,write = %d\n",tid,str_len); //检查是否到达CONTROL_GET
                }
            }
        }
        // todo 退出机制还需要修改
        printf("thread quit\n");
        close(clnt_sock);
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

//todo pthread detach

#endif //AGENTV2_THREADPOOL_H
