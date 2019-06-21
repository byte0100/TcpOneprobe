//
// Created by 54766 on 2019/5/21.
//

#ifndef AGENTV2_THREADPOOL_H
#define AGENTV2_THREADPOOL_H

#include "taskQueue.h"
#define MAX_THREADPOOL_SIZE 800

typedef struct ThreadPool{
    pthread_t pid[MAX_THREADPOOL_SIZE];     //线程池
    Queue* task_queue;      //任务消息队列

    pthread_mutex_t mutex;      //互斥锁 保护任务消息队列
    pthread_cond_t  cond;       //条件变量 通知子线程 任务消息队列 有新的任务（连接）
}ThreadPool;

void* pthread_func(void* arg);
char* Generate_str(char* pointer, int n);

void Init_Thread_pool(ThreadPool* pool, Queue* queue);

void Add_Task(ThreadPool* pool, Clnt_info clnt_info);


void* pthread_func(void* arg);

char* Generate_str(char* pointer, int n);

//todo pthread detach

#endif //AGENTV2_THREADPOOL_H
