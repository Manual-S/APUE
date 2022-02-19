#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include "proto.h"

int main(int argc,char* argv[])
{
    int sd;
    struct sockaddr_in raddr;
    struct msg_st sbuf;
    int errcode;

    if(argc < 2)
    {
        printf("argc < 2");
        exit(1);
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        printf("socket");
        exit(1);
    }
    strcpy(sbuf.name, "hello");
    sbuf.math = htonl(rand() % 100);
    sbuf.chinese = htonl(rand() % 100);

    raddr.sin_family = AF_INET;
    raddr.sin_port = htons(atoi(RCVPORT));

    inet_pton(AF_INET,argv[1],&raddr.sin_addr);

    errcode = sendto(sd, &sbuf, sizeof(sbuf), 0,(struct sockaddr *)&raddr, sizeof(raddr));

    if (errcode < 0)
    {
        printf("sendto error");
        exit(1);
    }
    close();

    return 0;
}