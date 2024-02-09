#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
void ErrorHandling(char *message);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    char message[30];
    int strLen = 0;
    int idx = 0, readLen = 0;

    if (argc != 3)
    {
        printf("Usage: %s <IP><port>\n", argv[0]);
        exit(1);
    }

    // 初始化 WinSock 库
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // 创建套接字
    hSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("socket() error!");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    // 客户端发出连接请求
    if (connect(hSocket, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("connect() error!");

    // 接收服务器发来的数据
    // 每次读取一个字节，strlen 每次加一，与 recv 调用次数相同
    while (readLen = recv(hSocket, &message[idx++], 1, 0))
    {
        if (readLen == -1)
            ErrorHandling("recv() error!");
        strLen += readLen;
    }

    printf("Message from server: %s \n", message);
    printf("Function read call count: %d \n", strLen);

    closesocket(hSocket);
    // 注销 WinSock 库
    WSACleanup();
    return 0;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}