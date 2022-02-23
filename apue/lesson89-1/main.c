#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
 #include <unistd.h>

#define THRNUM 4

static pthread_mutex_t mut[THRNUM];

static int next(int n)
{
    return n + 1 == THRNUM ? 0 : n + 1;
}

void* thr_func(void* p)
{
    int n = (int)p;
    int c = 'a' + (int)p;

    while(1)
    {
        pthread_mutex_lock(&mut[n]);
        write(1,&c,1);
        pthread_mutex_unlock(&mut[next(n)]);
    }

}

int main()
{
    pthread_t tid[THRNUM];
    int i = 0;
    int err = 0;
    for(i = 0;i < THRNUM;i++)
    {
        pthread_mutex_init(mut+i,NULL);
        pthread_mutex_lock(mut+i);
        err = pthread_create(tid+i,NULL,thr_func,(void*)i);
        if(err)
        {
            perror("pthread_create error");
            exit(1);
        }
   }
   pthread_mutex_unlock(mut+0);  // 释放0号锁
    alarm(5);

   for(i = 0;i < THRNUM;i++)
   {
       pthread_join(tid[i],NULL);
   }

   exit(0);
}