#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FNAME "/tmp/out"

void deamonize()
{
    int fd;
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        exit(1);
    }
    if(pid > 0)
    {
        // 说明是父进程
        exit(0);
    }

    fd = open("/dev/null",O_RDWR);
    if(fd < 0)
    {
        exit(1);
    }
    // 重定向
    dup2(fd,0);
    dup2(fd,1);
    dup2(fd,2);
    if(fd > 2)
    {
        close(fd);
    }

    setsid();

    // return 0;
}

int main()
{
    FILE* fp;
    int i = 0;
    deamonize();

    // 打开一个文件 让守护进程不停的向文件中写入数据
    fp = fopen(FNAME,"w");
    if (fp == NULL)
    {
        // 出错
        exit(1);
    }

    for(i = 0;;i++)
    {
        fprintf(fp,"%d\n",i);
        fflush(fp);
        sleep(1);
    }

    return 0;
}