#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<pthread.h>
#include "mytbf.h"

struct mytbf_st
{
    int cps;
    int burst;
    int token;
    int pos;  // 下标位置
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

static struct mytbf_st* job[MYTBF_MAX];
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;
pthread_t tid;

static void* thr_alrm(void* p)
{
    int i = 0;
    while(1)
    {
        pthread_mutex_lock(&mut_job));

        for(i = 0;i < MYTBF_MAX;i++)
        {
            if(job[i] != NULL)
            {
                pthread_mutex_lock(&job[i]->mut);
                job[i]->token = job[i]->token + job[i]->cps;
                if(job[i]->token > job[i]->cps)
                {
                    job[i]->token = job[i]->burst;
                }
                pthread_cond_broadcast(&job[i]->cond);
                pthread_mutex_unlock(&jon[i]->mut);
            }
        }

        pthread_mutex_unlock(&mut_job);
        sleep(1);
    }
}

int get_free_pos_ublocked()
{
    int i;
    for(i = 0;i < MYTBF_MAX;i++)
    {
        if(job[i] == NULL)
        {
            return i;
        }
    }

    return -1;
}

static void mould_unload(void)
{
    pthread_cancel(tid);
    pthread_join(tid);
    int i;
    for(i = 0;i < MYTBF_MAX;i++)
    {
        free(job[i]);
    }
}

static void mould_load(void)
{
    pthread_t tid;
    err = pthread_create(&tid,NULL,thr_alrm,NULL);
    if(err)
    {
        perror("mould_load pthread_create error");
        exit(1);
    }

    atexit(mould_unload);
}

mytbf_t* mytbf_init(int cps,int burst)
{
    struct mytbf_st *me;
    int pos;

    pthread_once();

    me = malloc(sizeof(*me));
    if (me == NULL)
    {
        return NULL;
    }
    me->cps = cps;
    me->token = 0;
    me->burst = burst;
    pthread_mutex_init(&me->mut,NULL);
    pthread_cond_init(&me->cond,NULL);

    pthread_mutex_lock(&mut_job);
    pos = get_free_pos_ublocked();
    if(pos < 0)
    {
        pthread_mutex_unlock(&mut_job);
        free(me);
        return NULL;
    }
    me->pos = pos;
    
    pthread_mutex_unlock(&mut_job);

    return me;
}
// 从ptr中取出size个token
int mytbf_fetchtoken(mytbf_t* ptr,int size)
{
    int n;
    struct mytbf_st* me = ptr;
    pthread_mutex_lock(&me->mut);
    while(me->token <= 0)
    {
        pthread_cond_wait(&me->cond,&me->mut);
    }
    n = min(me->token,size);
    me->token = me->token - n;
    pthread_mutex_unlock(&me->mut);

    return n;
}
//
int mytbf_returntoken(mytbf_t *ptr,int size)
{
    struct mytbf_st* me = ptr;
    pthread_mutex_lock(&me->mut);
    me->token = me->token + size;
    if(me->token > me->burst)
    {
        me->token = me->burst;
    }
    pthread_cond_broadcast();  // 这里写在lock前面了
    pthread_mutex_unlock(&me->mut);

    return 0;
}
int mytbf_destory(mytbf_t *ptr)
{
    struct mytbf_st* me = ptr;

    pthread_mutex_lock(&mut_job);
    job[me->pos] = NULL;
    pthread_mutex_unlock(&mut_job);


    pthread_mutex_destroy(&me->mut);
    pthread_cond_destroy(&me->cond);

    free(ptr);
    return 0;
}
