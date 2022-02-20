#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LINESIZE 1024
#define THRNUM 20  // 创建线程的个数
#define FNAME "/tmp/out"

/*
多个线程读取数据 然后累加1 再写会数据
*/
void* thr_add(void* p)
{
    char linebuf[LINESIZE];
    FILE* fp;
    // 打开
    fp = fopen(FNAME,"r+");
    if (fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    // 读数
    fgets(linebuf,LINESIZE,fp);
    
    fseek(fp,0,SEEK_SET);

    //
    fprintf(fp,"%d\n",atoi(linebuf) + 1);

    fclose(fp);

    pthread_exit(NULL);
}

int main()
{
    int i = 0;
    pthread_t tid[THRNUM];
    int err;

    for(i = 0;i < THRNUM;i++)
    {
        err = pthread_create(tid+i,NULL,thr_add,NULL);
        if(err)
        {
            fprintf(stderr,"%s",strerror(err));
            exit(1);
        }
    }

    for(i = 0;i <THRNUM;i++)
    {
        pthread_join(tid[i],NULL);
    }

    exit(0);
}
