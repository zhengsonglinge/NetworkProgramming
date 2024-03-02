#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHandling(char *message);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    char buf[BUF_SIZE];
    int strLen;
    int szClntAddr;

    fd_set read, except, readCopy, exceptCopy;
    struct timeval timeout;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error!");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(hServSock, (SOCKADDR *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error!");

    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error!");

    szClntAddr = sizeof(clntAddr);
    hClntSock = accept(hServSock, (SOCKADDR *)&clntAddr, &szClntAddr);

    FD_ZERO(&read);
    FD_ZERO(&except);
    FD_SET(hClntSock, &read);
    FD_SET(hClntSock, &except);

    while (1)
    {
        readCopy = read;
        exceptCopy = except;

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int result = select(0, &readCopy, 0, &exceptCopy, &timeout);

        if (result > 0)
        {
            // 如果有异常事件发生，即有紧急消息需要处理
            if (FD_ISSET(hClntSock, &exceptCopy))
            {
                strLen = recv(hClntSock, buf, BUF_SIZE - 1, MSG_OOB);
                buf[strLen] = 0;
                printf("Urgent message: %s \n", buf);
            }

            //  接收剩余的全部数据
            if (FD_ISSET(hClntSock, &readCopy))
            {
                strLen = recv(hClntSock, buf, BUF_SIZE - 1, 0);
                if (strLen == 0)
                {
                    closesocket(hClntSock);
                    break;
                }
                else
                {
                    buf[strLen] = 0;
                    puts(buf);
                }
            }
        }
    }

    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}