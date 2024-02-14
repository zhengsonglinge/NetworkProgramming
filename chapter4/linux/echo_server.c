#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    char message[BUF_SIZE];
    int str_len, i;

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 调用 socket 创建套接字
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                // 地址族（address family）IPv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 地址,INADDR_ANY 表示接受来自任何网络接口的连接
    serv_addr.sin_port = htons(atoi(argv[1]));     // 端口

    // 调用 bind 函数分配 IP 地址和端口号
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    // 调用 listen 函数将套接字转为可连接接收状态
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);
    // 循环调用 accept 函数，依次向客户端提供服务
    for (i = 0; i < 5; i++)
    {
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");
        else
            printf("Connected client %d \n", i + 1);

        // 实际完成回声服务，原封不动的传输读取的字符串
        while ((str_len = read(clnt_sock, message, BUF_SIZE)) != 0)
            write(clnt_sock, message, str_len);

        // 针对套接字调用close函数，向连接的相应套接字发送EOF。
        // 换言之，客户端套接字若调用 close 函数，则 while 条件变成假（false），执行服务端 close 代码
        close(clnt_sock);
    }

    // 向5个客户端提供服务后关闭服务器端套接字并终止程序。
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}