#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <ctype.h>
#define SERV_PORT 6666
#define SERV_IP "127.0.0.1"
int main()
{
    int lfd, cfd;
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    char buf[BUFSIZ];
    int n;
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(lfd, 32); // 同时允许32个客户端发起连接
    // accepte的第三个参数是一个传入传出参数
    // 调用完这个函数后 服务器端阻塞
    clie_addr_len = sizeof(clie_addr);
    cfd = accept(lfd, (sockaddr *)&clie_addr, &clie_addr_len);
    n = read(cfd, buf, sizeof(buf));
    for (int i = 0; i < n; i++)
    {
        buf[i] = toupper(buf[i]);
    }
    write(cfd, buf, n);
    close(lfd);
    close(cfd);
    return 0;
}
