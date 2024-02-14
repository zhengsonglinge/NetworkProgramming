#include <stdio.h>
#include <winsock2.h>

void ErrorHandling(char *message);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    unsigned short host_port = 0x1234; // 两个字节
    unsigned short net_port;
    unsigned long host_addr = 0x12345678; // 四个字节
    unsigned long net_addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    net_port = htons(host_port);
    net_addr = htonl(host_addr);

    printf("Host ordered port: %#x \n", host_port);
    printf("Newwork ordered port: %#x \n", net_port);
    printf("Host ordered address: %#lx \n", host_addr);
    printf("Newwork ordered address: %#lx \n", net_addr);

    WSACleanup();
    return 0;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}