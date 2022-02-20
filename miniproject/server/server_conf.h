#ifndef SERVER_CONF_H__
#define SERVER_CONF_H__

#define DEFAULT_MEDIADIR "/var/media"
#define DEFAULT_IF "enp0s3"

enum
{
    RUN_DAEMON = 1,  // 守护进程来运行
    RUN_FOREGROUND  // 前台运行
};

struct server_conf_st
{
    char* rcvport;
    char* mgroup;  
    char* media_dir;  // 媒体库相关的位置
    char runmode;  // 是否前台运行
    char* ifname;  // 记录走那个网卡
};

extern struct server_conf_st server_conf;

#endif