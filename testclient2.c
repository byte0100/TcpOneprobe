//
// Created by 54766 on 2019/6/5.
//

#include "TCPServer.h"
#include "MsgFormat.h"



typedef struct connect_info{
    int         datasize;
    in_addr_t   serv_ip;
    uint16_t    serv_port;

}Connect_info;

void* pthread_func(void* arg){
    Connect_info* connect_info = (Connect_info*)arg;
    int sock;
    struct sockaddr_in serv_addr;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        error_handling("sock() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = connect_info->serv_ip;
    serv_addr.sin_port = connect_info->serv_port;

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("connect() error");
    }

    MsgHeader myMsg1;
    memset(&myMsg1,0, sizeof(myMsg1));
    strcpy(myMsg1.messageHeader,"CC_O");
    myMsg1.controlMask = CONTROL_INIT;
    myMsg1.dataSize = connect_info->datasize;

    char buf[sizeof(myMsg1)] = {0};

    char* recvBuf = (char*)malloc(connect_info->datasize);
    bzero(recvBuf,connect_info->datasize);

    memcpy(buf,&myMsg1, sizeof(myMsg1));
    int ret = send(sock,buf, sizeof(myMsg1),0);
    printf("send_init size = %d\n", ret);
    if(ret < 0){
        error_handling("write() error");
    }

    ret = recv(sock,recvBuf, connect_info->datasize,0);
    if(ret < 0){
        error_handling("write() error");
    }
    printf("recv_INIT from server %s\n", recvBuf);

    MsgHeader myMsg2;
    memset(&myMsg2,0, sizeof(myMsg2));
    strcpy(myMsg2.messageHeader,"CC_O");
    myMsg2.controlMask = CONTROL_GET;
    memcpy(buf,&myMsg2, sizeof(myMsg2));
    ret = write(sock,buf, sizeof(myMsg2));
//    if(ret>0){
//        printf("GET write\n");
//    }
    if(ret < 0){
        error_handling("write() error");
    }
    int pos = 0;
    while(pos < connect_info->datasize){
        ret = recv(sock,recvBuf+pos, connect_info->datasize,0);
        pos += ret;
    }
//    ret = recv(sock,recvBuf, sizeof(recvBuf),0);
    if(ret < 0){
        error_handling("write() error");
    }
    printf("recv2 from server %s,size of recvBuf = %d\n", recvBuf, pos);
//    sleep(20);
    close(sock);

    return 0;
}

int main(int argc, char* argv[]){

    if(argc!=5){
        printf("Usage: %s <IP> <port> <datasize> <thread_num> \n", argv[0]);
        exit(-1);
    }

    Connect_info connect_info;
    connect_info.serv_ip = inet_addr(argv[1]);
    connect_info.serv_port = htons(atoi(argv[2]));
    connect_info.datasize = atoi(argv[3]);

    int thread_num = atoi(argv[4]);

    pthread_t tid;
    for (int i = 0; i < thread_num; ++i) {
        int ret = pthread_create(&tid,NULL,pthread_func,(void*)&connect_info);
        if(ret!=0){
            puts("pthread_create() error");
            return 1;
        }

    }
    return 0;
}