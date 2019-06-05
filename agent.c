//
// Created by 54766 on 2019/5/13.
// update v1.1 5/18 1.更新recv write函数的size参数（缓冲区大小）以及缓冲区变量名 #bug#
//                  2.实现probe端指定data大小的功能
//        v1.2 5/19 1.更新epoll多路复用功能
//              // todo 测试高并发，日志系统，代码规范，开发设计文档

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
    Clnt_info clnt_info;

    bzero(&clnt_info,0);

    struct  epoll_event *ep_events;
    struct  epoll_event event;
    int epfd, event_cnt;

    /* 初始化线程池 */
    queue_init(&queue);
    Init_Thread_pool(&pool,&queue);

    struct timeval tv;
    socklen_t optlen;


    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0){
        error_handling("socket() error");
    }

    optlen = sizeof(tv);
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    if(setsockopt(serv_sock,SOL_SOCKET,SO_RCVTIMEO,&tv,optlen) == -1){
        error_handling("setsockopt_SO_RCVTIMEO() error");
    }
    int snd_buf = 536*60;
    if(setsockopt(serv_sock,SOL_SOCKET,SO_SNDBUF,(void*)&snd_buf, sizeof(snd_buf))){
        error_handling("setsockopt_SO_SNDBUF() error");
    }

    socklen_t  len;
    len = sizeof(snd_buf);
    if(getsockopt(serv_sock,SOL_SOCKET,SO_SNDBUF,(void*)&snd_buf,&len)){
        error_handling("getsockopt() error");
    }
    printf("Send buffer size:%d \n", snd_buf);


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
                clnt_adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
                clnt_info.fd = clnt_sock;
                clnt_info.clnt_adr = clnt_adr;
                if(strcmp(argv[2],"-log") == 0){
                    clnt_info.log_flag = 1;
                    int log_fd = open("./testclient.log",O_RDWR|O_APPEND|O_CREAT,0666);
                    clnt_info.log_fd = log_fd;
                } else  clnt_info.log_flag = 0;
                if(clnt_sock > 0){
                    clnt_count++;
                    printf("%d client connect\n", clnt_count);//第clnt_count个客户端连接
                    clnt_info.clnt_id = clnt_count;
                    Add_Task(&pool,clnt_info);
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



