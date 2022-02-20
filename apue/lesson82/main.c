#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* func(void* p)
{
    puts("Thread is working!");
    return NULL;
}

int main()
{
    pthread_t tid;
    int err;

    puts("begin");

    err = pthread_create(&tid,NULL,func,NULL);
    if (err)
    {
        fprintf(stderr,"%s",strerror(err));
        exit(1);
    }
    sleep(10);  // 为了输出线程里面的那句话
    puts("end");
    return 0;
}

/*
gcc编译
gcc main.c -lpthread
因为pthread库不是标准linux库
*/