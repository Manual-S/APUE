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

mytbf_t* mytbf_init(int cps,int burst)
{
    struct mytbf_st *me;
    int pos;
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
    pthread_mutex_unlock(&me->mut)
}
//
int mytbf_returntoken(mytbf_t *)
{

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
