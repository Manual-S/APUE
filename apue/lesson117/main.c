#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define SIZE 1024

/*
父进程写管道 子进程读
*/
int main()
{
    int pd[2];
    pid_t pid;
    char buf[SIZE];
    int len = 0;
    if(pipe(pd) < 0)
    {
        error("pipe()");
        exit(1);
    }

    pid = fork();
    if(pid < 0)
    {
        // 出错
        perror();
        exit(1);
    }
    if(pid == 0)
    {
        // 说明是子进程
        close(pd[1]);
        len = read(pd[0],buf,SIZE);
        
        write(1,buf,len);  // 写到终端
        close(pd[0]);
        exit(0);
    }
    else
    {
        //父进程 写管道
        close(pd[0]);
        write(pd[1],"hello!",6);
        close(pd[1]);
        exit(0);
    }

    return 0;
}