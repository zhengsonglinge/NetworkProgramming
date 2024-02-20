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
    SOCKET servSock, clntSock;
    FILE *fp;
    int readCnt;
    SOCKADDR_IN servAddr, clntAddr;
    int clntAddrSize;
    char buf[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    // 打开文件
    fp = fopen("file_server_win.c", "rb");

    servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock == -1)
        ErrorHanding("socket() error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
        ErrorHanding("bind() error");

    if (listen(servSock, 5) == -1)
        ErrorHanding("listen() error");

    clntAddrSize = sizeof(clntAddr);
    clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrSize);
    if (clntSock == -1)
        ErrorHanding("accept() error");

    while (1)
    {
        // 读取文件数据，buf 表示数据读取后的存储位置，1 表示按字节读取，BUF_SIZE 表示每次读取数量，fp 是文件描述符
        readCnt = fread((void *)buf, 1, BUF_SIZE, fp);
        send(clntSock, buf, readCnt, 0);
        //  读取数量不足说明已经到达文件结尾
        if (readCnt < BUF_SIZE)
            break;
    }

    // 关闭输出流，同时发送 EOF,向客户端表示数据发送完毕
    shutdown(clntSock, SD_SEND);
    // 输入流未关闭，仍可接收客户端数据
    recv(clntSock, buf, BUF_SIZE, 0);
    printf("Message from client: %s \n", buf);

    fclose(fp);
    close(clntSock);
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