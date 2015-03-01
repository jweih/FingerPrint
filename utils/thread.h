#ifndef _THREAD_H
#define _THREAD_H

#include"common.h"


typedef struct task_t 
{
    void * (*task) (void *);
    void * args;
}task_t;


typedef struct node_t
{
    task_t *task;
    struct node_t *next;
	
} node_t;

typedef struct queue_t 
{
    node_t *head;
    node_t *tail;
} queue_t;




typedef struct threadpool_t 
{
    queue_t *task_queue;
    pthread_mutex_t qlock;
    int nthreads;
    int is_exit;
	int current_nthreads;
} threadpool_t;


struct thread_fun
{
	int (*init)(threadpool_t **tpool);
	int (*add_task)(threadpool_t *tpool, task_t *task);
	int (*run)(threadpool_t *tpool);
	int (*exit)(threadpool_t *tpool);
};

struct thread_fun * get_thread();


int test_thread(void);

int test_thread2();

#endif /*_THREAD_H*/