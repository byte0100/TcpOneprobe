//
// Created by 54766 on 2019/5/21.
//

#ifndef AGENTV2_TASKQUEUE_H
#define AGENTV2_TASKQUEUE_H

#include <stdbool.h>

#define MAX_LINK 1000


typedef struct  Queue{  //整型循环队列
    int front;
    int rear;
    int fd[MAX_LINK];
    int size;
}Queue;

void queue_init(Queue *queue){
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    bzero(queue->fd, sizeof(queue->fd));
}

bool queue_add(Queue* queue, int clnt_sock){
    if (queue->size == MAX_LINK) return  false; //队满
    queue->fd[queue->rear] = clnt_sock;
    queue->rear = (queue->rear + 1) % MAX_LINK;
    queue->size++;
    return  true;
}

int queue_pop(Queue* queue){    //返回队尾元素
    if (queue->size == 0) return -1; //队空
    int temp = -1;
    temp = queue->fd[queue->front];
    queue->front = (queue->front + 1) % MAX_LINK;
    queue->size--;
    return temp;
}

int queue_size(Queue* queue){   //队列当前长度
    return queue->size;
}

#endif //AGENTV2_TASKQUEUE_H
