#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "mytbf.h"

typedef void (*sighandler_t)(int);

static volatile int inited = 0;
static struct mytbf_st *job[MYTBF_MAX];
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static sighandler_t alrm_handler_save;
pthread_t tid_alarm;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

struct mytbf_st
{
    /* data */
    int cps;   // 传输速率
    int burst; // 令牌桶的上限
    int token; // token的数量
    int pos;
    pthread_mutex_t mut;
};

static int get_free_pos_unlocked(void)
{
    int i = 0;
    for (i = 0; i < MYTBF_MAX; i++)
    {
        if (job[i] == NULL)
        {
            return i;
        }
    }

    return -1;
}

void *thr_alarm(void *p)
{
    int i = 0;
    while (1)
    {
        pthread_mutex_lock(&mut_job);
        for (i = 0; i < MYTBF_MAX; i++)
        {
            if (job[i] != NULL)
            {
                pthread_mutex_lock(&job[i]->mut);
                job[i]->token = job[i]->token + job[i]->cps;
                if (job[i]->token >= job[i]->burst)
                {
                    job[i]->token = job[i]->burst;
                }
                pthread_mutex_unlock(&job[i]->mut);
            }
        }
        pthread_mutex_unlock(&mut_job);
        sleep(1);
        printf("thr_alarm\n");
    }
}

static void module_unload(void)
{
    int i = 0;
    // 恢复SIGALRM信号的默认处理方式
    // signal(SIGALRM, alrm_handler_save);

    // 把信号关闭
    // alarm(0);
    pthread_cancel(tid_alarm);
    pthread_join(tid_alarm, NULL);
    for (i = 0; i < MYTBF_MAX; i++)
    {
        if (job[i] != NULL)
        {
            mytbf_destory(job[i]);
        }
    }

    pthread_mutex_destroy(&mut_job);
}

void module_load()
{
    // alrm_handler_save = signal(SIGALRM, alrm_handler);
    // alarm(1);

    int err;
    err = pthread_create(&tid_alarm, NULL, thr_alarm, NULL);
    if (err)
    {
        exit(1);
    }
    atexit(module_load); // linux的钩子函数
}

mytbf_t *mytbf_init(int cps, int burst)
{
    struct mytbf_st *me;

    /*
        if (!inited)
        {
            module_load();
            inited = 1;
        }
    */
    pthread_once(&init_once, module_load);
    int pos;
    me = malloc(sizeof(*me));
    if (me == NULL)
    {
        return NULL;
    }

    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    pthread_mutex_init(&me->mut, NULL);

    pthread_mutex_lock(&mut_job); // 临界区越短越好
    pos = get_free_pos_unlocked();
    if (pos < 0)
    {
        // 令牌桶数组里已经没有可用的令牌桶
        pthread_mutex_unlock(&mut_job);
        free(me);
        return NULL;
    }
    me->pos = pos;
    job[pos] = me;
    pthread_mutex_unlock(&mut_job);

    return me;
}

int min(int a, int b)
{
    if (a > b)
    {
        return b;
    }
    return a;
}

int mytbf_fetchtoken(mytbf_t *ptr, int size) // 取令牌
{
    int n;
    struct mytbf_st *me = ptr;
    if (size <= 0)
    {
        // 参数不合法
        return -1;
    }

    pthread_mutex_lock(&me->mut);
    while (me->token <= 0)
    {
        // 加锁再释放锁
        pthread_mutex_unlock(&me->mut);
        sched_yield();
        pthread_mutex_lock(&me->mut);
    }

    n = min(me->token, size);

    pthread_mutex_unlock(&me->mut);
    me->token = me->token - n; // 表示从token中取走n个

    return n;
}
int mytbf_returntoken(mytbf_t *ptr, int size)
{
    struct mytbf_st *me = ptr;
    if (size <= 0)
    {
        return -1;
    }
    pthread_mutex_lock(&me->mut);
    me->token = me->token + size;
    if (me->token >= me->burst)
    {
        me->token = me->burst;
    }
    pthread_mutex_lock(&me->mut);
    return size;
}
int mytbf_destory(mytbf_t *ptr)
{
    struct mytbf_st *me = ptr;
    pthread_mutex_lock(&mut_job);
    job[me->pos] = NULL;

    pthread_mutex_unlock(&mut_job);
    pthread_mutex_destroy(&me->mut);
    free(ptr);
    return 0;
}