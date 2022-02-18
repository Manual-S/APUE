#include <stdio.h>
#include <stdlib.h>

/*
-M --mgroup  指定多播组
-P --port 指定接受端口
-p --player指定解码器
-H --help 显示帮助
*/

int main(int argc,char* argv[])
{
    /*命令行的分析*/
    /*
    初始化的级别
    默认值 配置文件 环境变量 命令行参数 
    */
    getopt_long();

    socket();

    pipe();
    /*
    fork创建出子进程
    */
    fork();
    
    /*
    子进程负责调用解码器
    父进程从网络上收包然后发送给子进程 通过写管道的方式
    */

    exit(0);
}