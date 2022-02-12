#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

#define CPS 10
#define BUFSIZE CPS
#define BURST 100

// static volatile int loop = 0;
static volatile sig_atomic_t token = 0; // sig_atomic_t信号原子类型

void *alarm_handler(int s)
{
    // alarm(1); setitimer可以周期性的发出信号
    token++;
    if (token > BURST)
    {
        token = BURST;
    }
}

int main(int argc, char **argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, ret, pos;
    struct itimerval itv;

    if (argc < 2)
    {
        printf("argc error\n");
        exit(1);
    }

    
    signal(SIGALRM, alarm_handler);
    //alarm(1);
    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;

    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;

    if(setitimer(ITIMER_REAL,&itv,NULL) < 0)
    {
        printf("setitimer error");
        exit(1);
    }

    do
    {
        /* code */
        sfd = open(argv[1], O_RDONLY);
        if (sfd < 0)
        {
            if (errno != EINTR)
            {
                printf("open error");
                exit(1);
            }
        }
    } while (sfd < 0);

    while (1)
    {
        /* code */

        while (token <= 0)
        {
            pause();
        }
        token--;

        /*
        上面有两个问题
        1. 判断完token <= 0后 还没有进入pause()的时候 信号到来了 那么token+1了，但是还是会pause()阻塞
        2. token--的同时 上面正好token++ 那这样token的值肯定是不对了
        */

        while ((len = read(sfd, buf, BUFSIZE)) < 0)
        {
            if (errno == EINTR)
            {
                printf("error = EINTR");
                continue;
            }
            perror("read()");
            break;
        }

        if (len == 0)
        {
            break;
        }
        pos = 0;
        while (len > 0)
        {
            ret = write(dfd, buf + pos, len);
            if (ret < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                perror("write()");
                exit(1);
            }
            pos += ret;
            len -= ret;
        }
        // sleep(1);
    }

    close(sfd);
    exit(0);
}


