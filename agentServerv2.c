//
// Created by 54766 on 2019/5/13.
//

#include "TCPServer.h"
#include "MsgFormat.h"

#define  BUF_SIZE 512

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

    if(listen(serv_sock, 10) == 5){
        error_handling("listen() error");
    }

    while(1){
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        if(clnt_sock > 0){
            clnt_count++;
            printf("%d client connect\n", clnt_count);//第clnt_count个客户端连接
            pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
            pthread_detach(t_id);

        }else{
            error_handling("accept() error");
        }
        //todo 退出机制
        printf("Connected client Port: %d \n", ntohs(clnt_adr.sin_port));
    }

    close(serv_sock);
    return 0;
}

void* handle_clnt(void* arg){
    int clnt_sock = *((int*) arg);
    int str_len;
    MsgHeader* clnt_msgheader = (MsgHeader*)malloc(sizeof(MsgHeader));

    int msgsize = sizeof(MsgHeader);
    char* buffer = (char*)malloc(msgsize);

    char* data;
    data = (char*) malloc(BUF_SIZE);

    while((str_len = recv(clnt_sock, buffer, sizeof(buffer), 0))){
//        printf("buffer = %s\n", buffer);
        memset(clnt_msgheader,0,msgsize);
        memcpy(clnt_msgheader, buffer, msgsize);
//        printf("recv from client %d messageHeader = %s controlMask = %d\n", clnt_sock, clnt_msgheader->messageHeader,
//                clnt_msgheader->controlMask); //检验是否正确解析收到的控制命令
        if(clnt_msgheader->messageHeader[3] == 'O'){
            if(clnt_msgheader->controlMask == CONTROL_INIT){   //Init
                char replybuf[4] = "ACK";
                str_len = write(clnt_sock, replybuf, sizeof(replybuf));
                Generate_str(data, BUF_SIZE); // 生成内容
//                printf("CONTROL_INIT, write = %d\n",str_len ); //检查是否到达CONTROL_INIT状态
            }

            if(clnt_msgheader->controlMask == CONTROL_ACK){
                write(clnt_sock, data, BUF_SIZE);
            }

            if(clnt_msgheader->controlMask == CONTROL_GET){
                int pos = 0;
                while (pos < BUF_SIZE){
                    str_len = send(clnt_sock, data+pos, sizeof(data),0);
                    pos += str_len;
                }
//                printf("CONTROL_GET, write = %d\n",str_len); //检查是否到达CONTROL_GET
            }
        }
    }
    close(clnt_sock);
    return NULL;
}

