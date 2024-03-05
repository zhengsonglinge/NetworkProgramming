#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024 // 传输消息字节数
#define RLT_SIZE 4    // 结果字节数
#define OPSZ 4        // 待操作数字节数

void ErrorHandling(char *message);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;
    char opmsg[BUF_SIZE];
    int result, opndCnt, i;

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
    else
        puts("Connected.......");

    // 读取待操作数个数并转换为 char 类型存储到 opmsg 中，占用 1 字节
    fputs("Operand count: ", stdout);
    scanf("%d", &opndCnt);
    opmsg[0] = (char)opndCnt;

    // 逐个读取待操作数，每个操作数占用 4 字节
    for (i = 0; i < opndCnt; i++)
    {
        printf("Opreand %d: ", i + 1);
        scanf("%d", (int *)&opmsg[i * OPSZ + 1]);
    }

    // 删掉缓冲区中的字符 \n
    fgetc(stdin);
    fputs("Operator: ", stdout);
    // 读取操作符并存储到 opmsg 中
    scanf("%c", &opmsg[opndCnt * OPSZ + 1]);
    // 发送到服务端
    send(hSocket, opmsg, opndCnt * OPSZ + 2, 0);
    // 接收服务端结果，4 字节
    recv(hSocket, &result, RLT_SIZE, 0);

    printf("Operation result: %d \n", result);
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