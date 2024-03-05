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
    int sock;
    char message[BUF_SIZE];
    struct sockaddr_in my_addr, your_addr;
    socklen_t addr_size;
    int str_len, i;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 调用 socket 创建套接字
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;                // 地址族（address family）IPv4
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 地址,INADDR_ANY 表示接受来自任何网络接口的连接
    my_addr.sin_port = htons(atoi(argv[1]));     // 端口

    // 调用 bind 函数分配 IP 地址和端口号
    if (bind(sock, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
        error_handling("bind() error");

    for (i = 0; i < 3; i++)
    {
        sleep(5);
        addr_size = sizeof(your_addr);
        // 接收来自客户端的消息
        // 同时获取数据客户端地址
        str_len = recvfrom(sock, message, BUF_SIZE, 0,
                           (struct sockaddr *)&your_addr, &addr_size);
        message[str_len] = 0;
        printf("Message %d: %s \n", i + 1, message);
    }

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}