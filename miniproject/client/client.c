#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include "client.h"
#include "../include/proto.h"

/*
-M --mgroup  指定多播组
-P --port 指定接受端口
-p --player指定解码器
-H --help 显示帮助
*/

struct client_conf_st client_conf = {
    .rcvport = DEFAULT_RCVPORT,
    .mgroup = DEFAULT_MGROUP,
    .player_cmd = DEFAULT_PLAYERCMD,
};

static void printhelp()
{
    printf("-P --Port 指定端口\n");
    printf("-M --mgroup 指定多播地址\n");
    printf("-p --player 指定播放器\n");
    printf("-H --help 显示帮助\n");
}
// 管道是尽力写 会照顾慢的一方
int writepipe(int fd, const void *buf, size_t len)
{
    int pos = 0;
    int count = 0;

    while (len > 0)
    {
        /* code */
        count = write(fd, buf + pos, len);
        if(count < 0)
        {
            return -1;
        }
        len = len - count;
        pos = pos + count;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int errcode = 0;
    int sd = 0;
    int index = 0;
    int c = 0;
    int pd[2];
    pid_t pid;
    int len;
    int chosenid;
    int ret;
    struct msg_channel_st *msg_channel;
    struct sockaddr_in server_addr;
    socklen_t server_addr_len;
    struct sockaddr_in laddr;
    struct sockaddr_in raddr;
    socklen_t raddr_len;
    struct ip_mreqn mreqn;
    struct option argarr[] = {{"port", 1, NULL, 'P'},
                              {"mgroup", 1, NULL, 'M'},
                              {"player", 1, NULL, 'p'},
                              {"help", 0, NULL, 'H'},
                              {NULL, 0, NULL, 0}};
    /*命令行的分析*/
    /*
    初始化的级别
    默认值 配置文件 环境变量 命令行参数
    */
    while (1)
    {
        c = getopt_long(argc, argv, "P:M:p:H", argarr, &index);
        if (c < 0)
        {
            break;
        }
        switch (c)
        {
        case 'P':
            /* code */
            break;
        case 'M':
            break;
        case 'p':
            break;
        case 'H':
            printhelp();
            exit(0);
            break;
        default:
            abort(); // 自己给自己发送一个失败信号
            break;
        }
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        perror("socket() error");
        exit(1);
    }
    /*client端加入一个多播组*/
    inet_pton(AF_INET,
              client_conf.mgroup,
              &mreqn.imr_multiaddr);

    inet_pton(AF_INET, "0.0.0.0", &mreqn.imr_address);
    mreqn.imr_ifindex = if_nametoindex("eth0");

    errcode = setsockopt(sd,
                         IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         &mreqn, sizeof(mreqn));
    if (errcode < 0)
    {
        printf("setsockopt error\n");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(client_conf.rcvport));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);

    errcode = bind(sd, (void*) & laddr, sizeof(laddr));
    if (errcode < 0)
    {
        printf("bind error\n");
        exit(1);
    }

    errcode = pipe(pd);
    if (errcode < 0)
    {
        perror("pipe error");
        exit(1);
    }
    /*
    fork创建出子进程
    */
    pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        exit(1);
    }
    /*
    子进程负责调用解码器
    父进程从网络上收包然后发送给子进程 通过写管道的方式
    */
    if (pid == 0)
    {
        // 子进程

        close(sd);
        close(pd[1]);   // 1 是写端
        dup2(pd[0], 0); // 重定向标准收入 播放器只能接受标准收入内容
        if (pd[0] > 0)
        {
            close(pd[0]);
        }

        execl("/bin/sh", "sh", "-c", client_conf.player_cmd), NULL;
        perror("execl() error");
        exit(1);
    }
    else
    {
        // 父进程
        // 收节目单
        // 选择频道
        // 收频道包 发送给子进程

        struct msg_list_st *msg_list;
        msg_list = malloc(MSG_LIST_MAX);

        if (msg_list == NULL)
        {
            perror("malloc error");
            exit(1);
        }

        while (1)
        {
            len = recvfrom(sd, msg_list, MSG_LIST_MAX, 0, (struct sockaddr *)&server_addr, &server_addr_len);
            if (len < sizeof(struct msg_list_st))
            {

                continue;
            }
            if (msg_list->chnid != LISTCHNID)
            {
                printf("chnid is not match\n");
                continue;
            }
            break;
        }

        // 打印节目单并选择频道
        struct msg_listentry_st *pos;
        for (pos = msg_list->entry;
             (char *)pos != (char *)(msg_list + len);
             pos = (void *)((char *)pos) + ntohs(pos->len))
            {
                printf("channel: %d:%s\n", pos->chnid, pos->desc);
            }

        free(msg_list);

        //用户输入选择的频道
        while (1)
        {
            ret = scanf("%d", &chosenid);
            if (ret != 1)
            {
                printf("chnnid id error\n");
                exit(1);
            }
        }

        msg_channel = malloc(MSG_CHANNEL_MAX);
        if (msg_channel == NULL)
        {
            exit(1);
        }

        while (1)
        {
            len = recvfrom(sd, msg_channel,
                           MSG_CHANNEL_MAX, 0,
                           (struct sockaddr *)&raddr, &raddr_len);

            if (raddr.sin_addr.s_addr != server_addr.sin_addr.s_addr ||
                raddr.sin_port != server_addr.sin_port)
            {
                printf("address not match\n");
                exit(1);
            }

            if (len < sizeof(struct msg_channel_st))
            {
                printf("massage too short \n");
                continue;
            }

            if (msg_channel->chnid == chosenid)
            {
                //
                printf("accept mesaage from %d", msg_channel->chnid);
                if ((writepipe(pd[1],msg_channel->data,len - sizeof(chnid_t)) < 0))
                {
                    printf("write pipe error\n");
                    exit(1);
                }
            }
        }
    }

    exit(0);
}