//
// Created by 54766 on 2019/5/13.
// update v1.1 5/18 1.更新recv write函数的size参数（缓冲区大小）以及缓冲区变量名 #bug#
//                  2.实现probe端指定data大小的功能
//        v1.2 5/19 1.更新epoll多路复用功能
//              todo 退出机制，线程退出，套接字关闭
//

#include "TCPServer.h"
#include "MsgFormat.h"
#include "taskQueue.h"
#include "ThreadPool.h"

#define EPOLL_SIZE 1000
#define EPOLL_WAITTIME 5

ThreadPool pool;
Queue queue;

int main(int argc, char* argv[]){
    int serv_sock, clnt_sock;
    struct  sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    int clnt_count = 0;


    struct  epoll_event *ep_events;
    struct  epoll_event event;
    int epfd, event_cnt;

    queue_init(&queue);
    Init_Thread_pool(&pool,&queue);

    struct timeval tv;
    socklen_t optlen;

    if(argc != 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0){
        error_handling("socket() error");
    }

    optlen = sizeof(tv);
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    if(setsockopt(serv_sock,SOL_SOCKET,SO_RCVTIMEO,&tv,optlen) == -1){
        error_handling("setsockopt() error");
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
        event_cnt = epoll_wait(epfd,ep_events,EPOLL_SIZE,EPOLL_WAITTIME);
        if(event_cnt == -1){
            puts("epoll_wait() error\n");
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
                    Add_Task(&pool,clnt_sock);
                }else{
                    puts("accept() error\n");
                }
            }
        }
//        printf("Connected client Port: %d \n", ntohs(clnt_adr.sin_port));
    }
    close(serv_sock);
    printf("close server socket\n");
    close(epfd);
    return 0;
}



