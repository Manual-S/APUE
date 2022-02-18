#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
int main()
{
    int count = 0;
    FILE *fp;
    

    while (1)
    {
        fp = fopen("tmp", "r");
        if (fp == NULL)
        {
            // fprintf(stderr,"fopen() failed! errno = %d\n",errno);
            // perror("fopen() error");  // 引号里的字段是自己规定的
            printf("count = %d\n",count);
            fprintf(stderr, "fopen error %s\n", strerror(errno)); // 第二种报错的方式
            exit(1);
        }
        count++;
    }

    printf("count = %d\n",count);


    exit(0);
}





