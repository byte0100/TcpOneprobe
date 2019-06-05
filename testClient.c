//
// Created by 54766 on 2019/5/13.
//

#include "TCPServer.h"
#include "MsgFormat.h"

//#define  DATA_SIZE 512 //测试客户端 指定生成的data总大小

int main(int argc, char* argv[]){
    int sock;
    struct sockaddr_in serv_addr;

    if(argc!=4){
        printf("Usage: %s <IP> <port> <datasize> \n", argv[0]);
        exit(-1);
    }

    int datasize;
    datasize = atoi(argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        error_handling("sock() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("connect() error");
    }

    MsgHeader myMsg1;
    memset(&myMsg1,0, sizeof(myMsg1));
    strcpy(myMsg1.messageHeader,"CC_O");
    myMsg1.controlMask = CONTROL_INIT;
    myMsg1.dataSize = datasize;
    char buf[sizeof(myMsg1)] = {0};
//    char recvBuf[packetsize] = {0};
    char* recvBuf = (char*)malloc(datasize);
    bzero(recvBuf,datasize);
    memcpy(buf,&myMsg1, sizeof(myMsg1));
    int ret = send(sock,buf, sizeof(myMsg1),0);
    printf("send_init size = %d\n", ret);
    if(ret < 0){
        error_handling("write() error");
    }

    ret = recv(sock,recvBuf, datasize,0);
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
    while(pos < datasize){
        ret = recv(sock,recvBuf+pos, datasize,0);
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