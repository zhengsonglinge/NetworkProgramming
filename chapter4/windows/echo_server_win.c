#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    char message[BUF_SIZE];
    int strLen, i;
    int szClntAddr;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // 初始化套接字库
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // 创建套接字
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error!");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    // 给套接字绑定地址和端口
    if (bind(hServSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error!");

    // 使套接字称为服务端套接字
    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error!");

    szClntAddr = sizeof(clntAddr);

    for (i = 0; i < 5; i++)
    {
        // 接收客户端连接请求
        hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szClntAddr);
        if (hClntSock == -1)
            ErrorHandling("accept() error!");
        else
            printf("Connected client %d \n", i + 1);

        // 接收客户端数据并转发给客户端
        while ((strLen = recv(hClntSock, message, BUF_SIZE, 0)) != 0)
            send(hClntSock, message, strLen, 0);

        closesocket(hClntSock);
    }
    closesocket(hServSock);
    // 注销初始化的套接字库
    WSACleanup();
    return 0;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}