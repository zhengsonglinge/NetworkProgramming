#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

void ErrorHandling(char *message);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    struct hostent *host;

    if (argc != 2)
    {
        printf("Usage: %s <addr> \n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // 根据域名获取 IP 地址
    host = gethostbyname(argv[1]);

    if (!host)
        ErrorHandling("gethost... error");

    // 输出官方域名
    printf("Official name: %s \n", host->h_name);
    // 输出官方域名以外的域名
    for (int i = 0; host->h_aliases[i]; i++)
        printf("Aliase %d: %s \n",
               i + 1,
               host->h_aliases[i]);
    // 地址类型
    printf("Address type: %s \n",
           (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
    // 输出 IP 地址
    for (int i = 0; host->h_addr_list[i]; i++)
        printf("IP addr %d: %s\n",
               i + 1,
               inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));

    WSACleanup();
    return 0;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}