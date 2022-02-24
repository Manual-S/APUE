// 基于信号实现的令牌桶
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "mytbf.h"

#define CPS 10
#define BUFSIZE 1024
#define BURST 100

// static volatile int loop = 0;
// static volatile int token = 0;

int main(int argc, char **argv)
{
    int sfd, dfd = 1;
    char buf[BUFSIZE];
    int len, ret, pos = 0;
    int size;
    mytbf_t *tbf;

    if (argc < 2)
    {
        printf("argc error\n");
        exit(1);
    }

    tbf = mytbf_init(CPS, BURST);
    if (tbf == NULL)
    {
        printf("mytbf_init error");
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

        size = mytbf_fetchtoken(tbf, BUFSIZE);
        if(size < 0)
        {
            printf("mytbf_fetchtoken error\n");
            exit(1);
        }
        
        while ((len = read(sfd, buf, size)) < 0)
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
            printf("\nlen = %d then break\n", len);
            break;
        }

        if (size - len > 0)
        {
            mytbf_returntoken(tbf, size - len);
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
    mytbf_destory(tbf);
    exit(0);
}