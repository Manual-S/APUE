#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include "proto.h"

/*
被动端 先运行
1 取得socket
2 给socket取得地址
3 收发消息
4 关闭socket

主动端
1 取得socket
2 给socket取得地址 可以省略
3 收发消息
4 关闭socket
*/

#define IPSTRSIZE 40

int main()
{
    int sd;
    struct sockaddr_in laddr;
    int ecode;
    struct msg_st rbuf;
    struct sockaddr_in raddr;
    socklen_t raddr_len;
    char ipstr[IPSTRSIZE];

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        printf("socket error");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(RCVPORT));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);

    ecode = bind(sd, (struct sockaddr *)&laddr, sizeof(laddr));
    if (ecode < 0)
    {
        printf("bind error");
        exit(1);
    }

    raddr_len = sizeof(raddr);

    while (1)
    {
        recvfrom(sd,&rbuf,sizeof(rbuf),0,(struct sockaddr *)&raddr,&raddr_len);
        inet_ntop(AF_INET,&raddr.sin_addr,ipstr,IPSTRSIZE);
        printf("---message from %s:%d\n",ipstr,ntohs(raddr.sin_port));
        printf("--name = %s\n",rbuf.name);
        printf("--math = %d\n",ntohl(rbuf.math));
        printf("--chinese = %d\n",ntohl(rbuf.chinese));
    }

    close(sd);

    exit(0);

    return 0;
}