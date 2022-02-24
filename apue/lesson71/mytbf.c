#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "mytbf.h"

typedef void (*sighandler_t)(int);

static volatile int inited = 0;
static struct mytbf_st* job[MYTBF_MAX];
static sighandler_t alrm_handler_save;

struct mytbf_st
{
    /* data */
    int cps;   // 传输速率
    int burst; // 令牌桶的上限
    int token; // token的数量
    int pos;
};

static int get_free_pos(void)
{
    int i = 0;
    for(i = 0;i < MYTBF_MAX;i++)
    {
        if(job[i] == NULL)
        {
            return i;
        }
    }

    return -1;
}

void* alrm_handler(int s)
{
    printf("alrm_handler \n");
    int i = 0;
    alarm(1);
    for(i = 0;i < MYTBF_MAX;i++)
    {
        if(job[i] != NULL)
        {
            job[i]->token = job[i]->token + job[i]->cps;
            if(job[i]->token >= job[i]->burst)
            {
                job[i]->token = job[i]->burst;
            }
        }
    }
}

static void module_unload(void)
{
    int i = 0;
    // 恢复SIGALRM信号的默认处理方式
    signal(SIGALRM,alrm_handler_save);

    // 把信号关闭
    alarm(0);

    for(i = 0;i < MYTBF_MAX;i++)
    {
        free(job[i]);
    }
}

void module_load()
{
    alrm_handler_save = signal(SIGALRM,alrm_handler);
    alarm(1);

    atexit(module_load);  // linux的钩子函数
}

mytbf_t *mytbf_init(int cps,int burst)
{
    struct mytbf_st *me;
    
    if(!inited)
    {
        module_load();
        inited = 1;
    }

    int pos;
    pos = get_free_pos();
    if (pos < 0)
    {
        // 令牌桶数组里已经没有可用的令牌桶
        return NULL;
    }

    me = malloc(sizeof(*me));
    if (me == NULL)
    {
        return NULL;
    }

    me->token = 0;
    me->cps = cps;
    me->burst = burst;
    me->pos = pos;

    job[pos] = me;

    return me;
}

int min(int a,int b)
{
    if (a > b)
    {
        return b;
    }
    return a;
}

int mytbf_fetchtoken(mytbf_t *ptr,int size)  // 取令牌
{
    int n;
    struct mytbf_st *me = ptr;
    if (size <= 0)
    {
        // 参数不合法
        return -1;
    }

    while(me->token <= 0)
    {
        /*
        这里用whiel不用if的原因是
        pause()可能被信号中断 但是被信号中断并不代表当前的token >= 0 
        所以这里要用while来等待
        */
        pause();
    }

    n = min(me->token,size);

    me->token = me->token - n;  // 表示从token中取走n个

    return n;
}
int mytbf_returntoken(mytbf_t *ptr,int size)
{
    struct mytbf_st *me = ptr;
    if(size <= 0)
    {
        return -1;
    }
    
    me->token = me->token + size;
    if(me->token >= me->burst)
    {
        me->token = me->burst;
    }

    return size;
}
int mytbf_destory(mytbf_t *ptr)
{
    struct mytbf_st* me = ptr;
    job[me->pos] = NULL;
    free(ptr);
    return 0;
}