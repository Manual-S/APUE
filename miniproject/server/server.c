#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "server_conf.h"
#include "thr_channel.h"

/*
-M 指定多播组
-P 指定接收端口号
-F 前台运行
-D 指定媒体库运行
-H 显示帮助
*/

struct server_conf_st server_conf = {
    .mgroup = DEFAULT_MGROUP,
    .media_dir = DEFAULT_MEDIADIR,
    .runmode = RUN_DAEMON,
    .ifname = DEFAULT_IF};

static void print_help()
{
    printf("-P 配置端口\n");
    printf("-M 配置多播组\n");
    printf("-F 配置运行模式\n");
    printf("-D 配置媒体库\n");
    printf("-I 设置网卡\n");
    printf("-H 帮助信息\n");
}

static int socket_init()
{
    int serversd;
    struct ip_mreqn mreq;
    int errcode;

    inet_pton(AF_INT, server_conf.mgroup, &mreq.imr_multiaddr);
    inet_pton(AF_INT, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex(server_conf.ifname);

    serversd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serversd < 0)
    {
        syslog(LOG_ERR, "socket() %s", strerror(errno));
        exit(1);
    }

    errcode = setsockopt(serversd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq));
    if (errcode < 0)
    {
        syslog(LOG_ERR, "setsockopt %s", strerror(errno));
        exit(1);
    }
}

// 信号响应函数
static void *daemon_exit(int s)
{
    syslog(LOG_WARNING, "signal -%d caught,exit now", s);
    closelog();
    exit(0);
}

static int daemonize()
{
    pid_t pid;
    int fd;
    pid = fork();

    if (pid < 0)
    {
        printf("fork error\n");
        return -1;
    }

    if (pid > 0)
    {
        // 父进程
        syslog(LOG_ERR, "fork() failed:%s", strerror(errno));
        exit(0);
    }

    // 子进程

    // 对默认输入输出进行重定向
    fd = open("/dev/null", O_RDWR);
    if (fd < 0)
    {
        syslog(LOG_ERR, "open() failed:%s", strerror(errno));
        return -2;
    }
    else
    {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);

        if (fd > 2)
        {
            close(fd);
        }
    }

    chdir("/");
    umask(0);

    setsid(); // 父进程直接退出 子进程被init进程接收 成为守护进程
    return 0;
}

int main(int argc, char *argv[])
{
    int c = 0;
    int errcode = 0;
    struct sigaction sa;

    // 守护进程是有信号来打断的 因此设置信号
    sa.sa_handler = daemon_exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    // 系统日志
    openlog("netradio", LOG_PID | LOG_PERROR, LOG_DAEMON);

    /*命令行分析*/
    while (1)
    {
        /* code */
        c = getopt(argc, argv, "M:P:FD:I:H:");
        if (c < 0)
        {
            break;
        }
        switch (c)
        {
        case 'M':
            server_conf.mgroup = optarg;
            break;
        case 'P':
            server_conf.rcvport = optarg;
            break;
        case 'F':
            server_conf.runmode = RUN_FOREGROUND;
            break;
        case 'D':
            server_conf.media_dir = optarg;
            break;
        case 'I':
            server_conf.ifname = optarg;
            break;
        case 'H':
            print_help();
            break;
        default:
            abort();
            break;
        }
    }

    /*当前进程变成守护进程 当进程变为守护进程后，日志要进行重定向*/
    if (server_conf.runmode == RUN_DAEMON)
    {
        // 实现为后台进程
        if (daemonize() == 0)
        {
            exit(1);
        }
    }
    else if (server_conf.runmode == RUN_FOREGROUND)
    {
    }
    else
    {

        exit(1);
    }

    /*socket初始化*/
    socket_init();

    /*获取频道信息*/
    struct media_listentry_st *list;
    int list_size = 0;
    errcode = mlib_getchnlist(list, list_size);
    if (errcode < 0)
    {
    }
    /*创建节目单线程*/
    errcode = thr_list_create(list, list_size);
    if (errcode < 0)
    {
    }
    /*创建频道线程*/

    for(i = 0;i < list_size;i++)
    {
        thr_channel_create(list+i);
    }

    

    exit(0);
}