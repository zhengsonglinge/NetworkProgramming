#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHanding(char *message);

int main(int argc, char const *argv[])
{

    WSADATA wsaData;
    int sock;
    SOCKADDR_IN servAddr;
    char message[BUF_SIZE];
    int strLen;
    int addrSize;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port> \n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    // 使用 SOCK_DGRAM 表示 UDP 协议
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        ErrorHanding("socket() error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;                 // 地址族（address family）IPv4
    servAddr.sin_addr.s_addr = inet_addr(argv[1]); // 地址
    servAddr.sin_port = htons(atoi(argv[2]));      // 端口

    connect(sock, (SOCKADDR *)&servAddr, sizeof(servAddr));

    while (1)
    {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        // 将字符串发送到服务端
        send(sock, message, strlen(message), 0);
        // 接收服务端数据
        strLen = recv(sock, message, BUF_SIZE, 0);
        message[strLen] = 0;
        printf("Message from server: %s", message);
    }

    WSACleanup();
    close(sock);
    return 0;
}

void ErrorHanding(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}