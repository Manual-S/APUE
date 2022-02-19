#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "server_conf.h"

/*
-M 指定多播组
-P 指定接收端口号
-F 前台运行
-D 指定媒体库运行
-H 显示帮助
*/

static void print_help()
{
    printf("-P 配置端口");
}

int main(int argc,char* argv[])
{
    int c = 0;
    /*命令行分析*/
    while (1)
    {
        /* code */
        c = getopt_long(argc,argv,"");
        if (c < 0)
        {
            break;
        }
        switch (c)
        {
        case /* constant-expression */:
            /* code */
            break;
        
        default:
            break;
        }
    }
    
    /*当前进程变成守护进程 当进程变为守护进程后，日志要进行重定向*/


    /*socket初始化*/

    /*获取频道信息*/

    /*创建节目单线程*/

    /*创建频道线程*/

    exit(0);
}