
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port> \n", argv[0]);
        exit(1);
    }
    // 创建套接字，但此时套接字并不马上分为服务器端和客户端。
    // 如果紧接着调用 bind、listen 函数，将成为服务器端套接字；
    // 如果调用connect函数，将成为客户端套接字。
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                 // 地址族（address family）IPv4
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); // 地址
    serv_addr.sin_port = htons(atoi(argv[2]));      // 端口

    // 调用connect函数向服务器端发送连接请求。
    if (connect(&serv_addr, 0, sizeof(serv_addr)))
        error_handling("connnect() error");

    str_len = read(sock, message, sizeof(message) - 1);
    if (str_len == -1)
        error_handling("read() error");

    printf("Message from server : %s \n", message);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}