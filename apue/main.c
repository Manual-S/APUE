#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
int main()
{
    FILE *fp;
    fp = fopen("tmp","w+");
    if(fp == NULL)
    {
        //fprintf(stderr,"fopen() failed! errno = %d\n",errno);
        //perror("fopen() error");  // 引号里的字段是自己规定的
        fprintf(stderr,"fopen error %s\n",strerror(errno));  // 第二种报错的方式
        exit(1);
    }

    puts("ok");

    fclose(fp);

    exit(0);
}
