#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define TTL 64
#define BUF_SIZE 30

void ErrorHandling(char *message);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    SOCKET sendSock;
    SOCKADDR_IN mulAddr;
    int timeLive = TTL;
    FILE *fp;
    char buf[BUF_SIZE];

    if (argc != 3)
    {
        printf("Usage: %s <GroupIP> <Port> \n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // 多播通过 UDP 实现的，因此创建 UDP 套接字
    sendSock = socket(PF_INET, SOCK_DGRAM, 0);

    // 设置传输数据的目标地址信息，必须将 IP 地址设置为多播地址
    memset(&mulAddr, 0, sizeof(mulAddr));
    mulAddr.sin_family = AF_INET;
    mulAddr.sin_addr.s_addr = inet_addr(argv[1]); // 多播 IP
    mulAddr.sin_port = htons(atoi(argv[2]));      // 多播端口

    // 指定套接字 TTL 信息
    setsockopt(sendSock, IPPROTO_IP,
               IP_MULTICAST_TTL, (void *)&timeLive, sizeof(timeLive));

    if ((fp = fopen("news_sender_win.c", "r")) == NULL)
        ErrorHandling("fopen() error");

    // 读取到 EOF 终止，即读取到文件结束
    while (!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        // 基于 UDP 传输数据，因此使用 sendto 函数传输数据
        sendto(sendSock, buf, strlen(buf),
               0, (SOCKADDR *)&mulAddr, sizeof(mulAddr));
        sleep(2);
    }
    fclose(fp);
    close(sendSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
