/*************************************************************************
*�����ⲿ�ִ�����Ҫ���̳߳ص�ʵ�֣�����ο��������Լ�linux kernel
**************************************************************************/

#include"thread.h"

#undef  DBG_ON
#undef  FILE_NAME
#define DBG_ON  	(0x01)
#define FILE_NAME 	"thread:"

#define DEFAULT_N_THREADS  (32u)


/***************************************************************************
*��������		:	�̵߳Ĵ���
*����thread_id	:	�߳�id
*����pthread_fun	:	�̷߳�����ָ��
*����arg			:	���ݸ�pthread_fun�Ĳ���
*����stack_size	:	�̶߳�ջ��С
*����priority		:	�߳����ȼ�
*����ֵ			:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int thread_create(pthread_t *thread_id, void* (*pthread_fun)(void* param), void *arg, int stack_size, int priority)
{
    
    int ret;
    pthread_attr_t SchedAttr;
    struct sched_param	SchedParam;
    
    pthread_attr_init( &SchedAttr );				
    memset(&SchedAttr, 0, sizeof(pthread_attr_t));		
    if(priority != -1)
    {
        SchedParam.sched_priority = priority;	
        pthread_attr_setschedparam( &SchedAttr, &SchedParam );
        pthread_attr_setschedpolicy( &SchedAttr, SCHED_RR );
    }
    pthread_attr_setstacksize( &SchedAttr, stack_size );
    ret = pthread_create( thread_id, &SchedAttr, anyka_thread_main, arg);
    pthread_attr_destroy(&SchedAttr);

    return ret;
    
}


/***************************************************************************
*��������	:	�������Ƿ�Ϊ��
*����q		:	����
*����ֵ		:	1==��  0==�ǿ�
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int queue_is_empty(queue_t *q) 
{
        if(!q)
                return -2;
        return (q->head == NULL);
}


/***************************************************************************
*��������	:	��ʼ������
*����q		:	����ָ���ָ��
*����ֵ		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int queue_init(queue_t **q) 
{
        *q = (queue_t *) malloc(sizeof(queue_t));
        if(*q == NULL) 
                return -ENOMEM;
        memset(*q, 0, sizeof(queue_t));
        return 0;
}

/***************************************************************************
*��������	:	���һ�����񵽶���
*����q		:	����
*����task	:	Ҫ��ӵ�����
*����ֵ		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int queue_add(queue_t *q, task_t *task) 
{
        node_t *current;
        if(q == NULL)
                return -1;
        current = (node_t *) malloc(sizeof(node_t));
        if(current == NULL) 
                return -1;
        current->task = task;
        if(queue_is_empty(q))
                q->head = current;
        else 
                q->tail->next = current;
        q->tail = current;
        return 0;
}


/***************************************************************************
*��������	:	�Ӷ����л�ȡһ������
*����q		:	����
*����task	:	��ȡ������
*����ֵ		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int queue_remove(queue_t *q, task_t *task)
{
        if(q == NULL) 
                return -2;
        if(queue_is_empty(q)) 
                return -1;
        else 
		{
                node_t *old_head = q->head;
                *task = *(old_head->task);
                q->head = q->head->next;
                if(q->head == NULL)
                        q->tail = NULL;
             //   free(old_head);    /*һ���Է���*/
        }
        return 0;
}



/***************************************************************************
*��������	:	���ٶ���
*����q		:	Ҫ���ٵĶ���
*����ֵ		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int queue_destroy(queue_t **q) 
{
	if(NULL != *q)
    free(*q);
    return 0;
}



/***************************************************************************
*��������	:	�����̵߳ķ�����
*����arg		:	���ݸ��̵߳Ĳ���
*����ֵ		:	(void *)0
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static void * worker_loop(void *arg) 
{
        threadpool_t *tpool = (threadpool_t *) arg;
        pthread_mutex_t *lock = &(tpool->qlock);
		if(NULL == tpool)
		{
			dbg_printf("tpool is null \n");
			return((void*)0);
		}
        while(1)
		{
                while(queue_is_empty(tpool->task_queue) && !tpool->is_exit)
                {
					sleep(1);
				}
                if(tpool->is_exit)
				{
                        pthread_exit(NULL);
                        return (void *) 0;
                }
                pthread_mutex_lock(lock);
                if(queue_is_empty(tpool->task_queue)) 
				{
                        pthread_mutex_unlock(lock);
                        continue;
                } 
                else 
				{
                        task_t task;
                        int rc = queue_remove(tpool->task_queue, &task);
						
                        if(rc < 0) 
						{
                                dbg_printf("Error removing task from task queue: %d.", rc);
                                pthread_mutex_unlock(lock);
                                continue;
                        }
                        else
						{

                                pthread_mutex_unlock(lock);
                                task.task(task.args);
					
                        }
						 
                } 
				
        }
        return (void *) 0;
}


/***************************************************************************
*��������	:	�̳߳صĳ�ʼ��
*����tpool	:	��Ҫ��ʼ�����̳߳�
*����ֵ		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int threadpool_init(threadpool_t **tpool)
{
        int rc;
        *tpool = (threadpool_t *)malloc(sizeof(threadpool_t));
        if(*tpool == NULL) 
        {
			dbg_printf("malloc fail \n");
			return(1);
		}
        memset(*tpool, 0, sizeof(threadpool_t));
        rc = queue_init(&((*tpool)->task_queue));
        if(rc < 0)
        {
        	dbg_printf("queue_init fail \n");
			return rc;
		}
                
        pthread_mutex_init(&((*tpool)->qlock), NULL);
        (*tpool)->nthreads = DEFAULT_N_THREADS;

        return 0;
}



/***************************************************************************
*��������	:	���̳߳����������
*����tpool	:	�̳߳�
*����task	:	Ҫ��ӵ�����
*����ֵ		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int  threadpool_add_task(threadpool_t *tpool, task_t *task)
{
        pthread_mutex_t *lock = &(tpool->qlock);
        pthread_mutex_lock(lock);
        queue_add(tpool->task_queue, task);
		tpool->current_nthreads += 1;
        pthread_mutex_unlock(lock);
        return 0;
}



/***************************************************************************
*��������	:	�����������߳�
*����tpool	:	�̳߳�
*����ֵ		:	0==ok  others==wrong
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int threadpool_run(threadpool_t *tpool) 
{
        int i=0;
		int n=0;
		int ret = 0;
        if(tpool == NULL) 
                return -1;
		if(tpool->current_nthreads > tpool->nthreads)
		{
			n = tpool->nthreads;
			dbg_printf("pthread num out of the limit \n");
		}
		else
		{
			n = tpool->current_nthreads;
		}

        pthread_t *threads = (pthread_t *) malloc(sizeof(pthread_t) * n);
		if(NULL == threads )
		{
			dbg_printf("malloc fail \n");
			return(1);

		}
        for(i = 0; i < n; i++)
		{
			ret=thread_create(&threads[i],worker_loop,(void *)tpool,100*1024,-1);
			if(ret != 0 )
			dbg_printf("thread_create fail !\n");
        }

        return 0;
}


/***************************************************************************
*��������	:	�����߳�
*����tpool	:	�̳߳�
*����ֵ		:	0
*--------------------------------------------------------------------------
*2015/03/01		jweih 			����
****************************************************************************/

static int threadpool_exit(threadpool_t *tpool)
{
        tpool->is_exit = 1;
        return 0;
}





struct thread_fun thread={
	.init 		= threadpool_init,
	.add_task 	= threadpool_add_task,
	.run 		= threadpool_run,
	.exit 		= threadpool_exit,
};


struct thread_fun * get_thread()
{
	return(&thread);
}





