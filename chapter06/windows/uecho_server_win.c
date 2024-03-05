#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHanding(char *message);

int main(int argc, char const *argv[])
{
    WSADATA WSAData;
    SOCKET servSock;
    SOCKADDR_IN servAddr, clntAddr;
    char message[BUF_SIZE];
    int strLen;
    int clntAddrSize;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
        ErrorHanding("WSAStartup() error!");

    // 调用 socket 创建套接字
    servSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (servSock == INVALID_SOCKET)
        ErrorHanding("socket() error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;                // 地址族（address family）IPv4
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 地址,INADDR_ANY 表示接受来自任何网络接口的连接
    servAddr.sin_port = htons(atoi(argv[1]));     // 端口

    // 调用 bind 函数分配 IP 地址和端口号
    if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
        ErrorHanding("bind() error");

    while (1)
    {
        clntAddrSize = sizeof(clntAddr);
        // 接收来自客户端的消息
        // 同时获取数据客户端地址
        strLen = recvfrom(servSock, message, BUF_SIZE, 0,
                          (struct sockaddr *)&clntAddr, &clntAddrSize);
        // 将消息同时转发给客户端
        // 客户端地址是通过 recvfrom 函数获取的
        sendto(servSock, message, strLen, 0,
               (struct sockaddr *)&clntAddr, sizeof(clntAddr));
    }

    // 不会执行，因为上面是 while(1) 且没有 break 语句
    close(servSock);
    WSACleanup();
    return 0;
}

void ErrorHanding(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}