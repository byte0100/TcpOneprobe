//
// Created by 54766 on 2019/5/21.
//

#ifndef AGENTV2_TASKQUEUE_H
#define AGENTV2_TASKQUEUE_H

#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_LINK 1000

typedef  struct clnt_info{
    int fd;     // client sock fd
    struct sockaddr_in clnt_adr;      //client address
}Clnt_info;

typedef struct  Queue{  //整型循环队列 -》结构数组队列
    int front;
    int rear;
    Clnt_info clnt_info[MAX_LINK];
    int size;
}Queue;

void queue_init(Queue *queue);

bool queue_add(Queue* queue, Clnt_info clnt_sock);

int queue_pop(Queue* queue, Clnt_info* clnt_info);

int queue_size(Queue* queue);

#endif //AGENTV2_TASKQUEUE_H
