#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int serv_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char message[BUF_SIZE];
    int str_len;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 调用 socket 创建套接字
    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                // 地址族（address family）IPv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 地址,INADDR_ANY 表示接受来自任何网络接口的连接
    serv_addr.sin_port = htons(atoi(argv[1]));     // 端口

    // 调用 bind 函数分配 IP 地址和端口号
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        // 接收来自客户端的消息
        // 同时获取数据客户端地址
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0,
                           (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        // 将消息同时转发给客户端
        // 客户端地址是通过 recvfrom 函数获取的
        sendto(serv_sock, message, str_len, 0,
               (struct sockaddr *)&clnt_addr, clnt_addr_size);
    }

    // 不会执行，因为上面是 while(1) 且没有 break 语句
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}