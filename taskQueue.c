//
// Created by 54766 on 2019/5/26.
//

#include "taskQueue.h"

void queue_init(Queue *queue){
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    bzero(queue->clnt_info, sizeof(queue->clnt_info));
}

bool queue_add(Queue* queue, Clnt_info clnt_info){
    if (queue->size == MAX_LINK) return  false; //队满
    queue->clnt_info[queue->rear] = clnt_info;
    queue->rear = (queue->rear + 1) % MAX_LINK;
    queue->size++;
    return  true;
}

int queue_pop(Queue* queue, Clnt_info* clnt_info){    //返回队尾元素
    if (queue->size == 0) return -1; //队空
    *clnt_info = queue->clnt_info[queue->front];
    queue->front = (queue->front + 1) % MAX_LINK;
    queue->size--;
    return 0;
}

int queue_size(Queue* queue){   //队列当前长度
    return queue->size;
}