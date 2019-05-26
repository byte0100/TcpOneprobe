//
// Created by 54766 on 2019/5/21.
//

#ifndef AGENTV2_TASKQUEUE_H
#define AGENTV2_TASKQUEUE_H

#include <stdbool.h>
#include <string.h>

#define MAX_LINK 1000


typedef struct  Queue{  //整型循环队列
    int front;
    int rear;
    int fd[MAX_LINK];
    int size;
}Queue;

void queue_init(Queue *queue);

bool queue_add(Queue* queue, int clnt_sock);

int queue_pop(Queue* queue);

int queue_size(Queue* queue);

#endif //AGENTV2_TASKQUEUE_H
