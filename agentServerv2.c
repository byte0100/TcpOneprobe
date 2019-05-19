//
// Created by 54766 on 2019/5/13.
// update v1.1 5/18 1.更新recv write函数的size参数（缓冲区大小）以及缓冲区变量名 #bug#
//                  2.实现probe端指定data大小的功能
//        v1.2 5/19 1.更新epoll多路复用功能
//                  2.todo 线程池
//

#include "TCPServer.h"
#include "MsgFormat.h"

//#define  DATA_SIZE 512
#define EPOLL_SIZE 1000

void* handle_clnt(void* arg);

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

int main(int argc, char* argv[]){
    int serv_sock, clnt_sock;
    struct  sockaddr_in serv_adr, clnt_adr;
    pthread_t  t_id;
    int clnt_adr_sz;
    int clnt_count = 0;

    struct  epoll_event *ep_events;
    struct  epoll_event event;
    int epfd, event_cnt;



    if(argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0){
        error_handling("socket() error");
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1){
        error_handling("bind() error");
    }

    if(listen(serv_sock, 10) == -1){
        error_handling("listen() error");
    }

    if((epfd = epoll_create(EPOLL_SIZE) )== -1){
        error_handling("epoll_create() error");
    }
    ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);

    while(1){
        event_cnt = epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
        if(event_cnt == -1){
            puts("epoll_wait() error");
            break;
        }

        for(int i =0; i<event_cnt; i++){
            if(ep_events[i].data.fd == serv_sock){
                // todo
                clnt_adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
                if(clnt_sock > 0){
                    clnt_count++;
                    printf("%d client connect\n", clnt_count);//第clnt_count个客户端连接
                    pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
                    pthread_detach(t_id); //可以交由子线程自己detach agent主线程应该不需要子线程的返回值

                }else{
                    error_handling("accept() error");
                }
            }
        }

//        clnt_adr_sz = sizeof(clnt_adr);
//        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
//        if(clnt_sock > 0){
//            clnt_count++;
//            printf("%d client connect\n", clnt_count);//第clnt_count个客户端连接
//            pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
//            pthread_detach(t_id);
//
//        }else{
//            error_handling("accept() error");
//        }
        //todo 退出机制
        printf("Connected client Port: %d \n", ntohs(clnt_adr.sin_port));
    }

    close(serv_sock);
    close(epfd);
    return 0;
}

void* handle_clnt(void* arg){
    int clnt_sock = *((int*) arg);
    int str_len;
    int dataSize = 0; // 生成的data总大小
    MsgHeader* clnt_msgheader = (MsgHeader*)malloc(sizeof(MsgHeader));


    int msgsize = sizeof(MsgHeader);
//    printf("msgsize = %d\n", msgsize); // 测试msgsize
    char* msgbuffer = (char*)malloc(msgsize);

    char* data;
//    data = (char*) malloc(DATA_SIZE); 移动到line 98

    while((str_len = recv(clnt_sock, msgbuffer, msgsize, 0))){
        printf("msgbuffer = %s\n", msgbuffer);
        printf("recv size = %d\n", str_len);
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
                printf("CONTROL_INIT, write = %d\n",str_len ); //检查是否到达CONTROL_INIT状态
            }

            if(clnt_msgheader->controlMask == CONTROL_ACK){
                send(clnt_sock, data, dataSize, 0);
            }

            if(clnt_msgheader->controlMask == CONTROL_GET){
                str_len = send(clnt_sock, data, dataSize,0);  // todo 修改了之前的问题 还没测试 可能需要循环发送？
                printf("CONTROL_GET, write = %d\n",str_len); //检查是否到达CONTROL_GET
            }
        }
    }
    close(clnt_sock);
    return NULL;
}

