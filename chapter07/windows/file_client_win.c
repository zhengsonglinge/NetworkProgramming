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
    FILE *fp;
    struct sockaddr_in servAddr;
    char buf[BUF_SIZE];
    int readCnt;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port> \n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    // 打开待写入文件
    fp = fopen("receive.dat", "wb");

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        ErrorHanding("socket() error");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
        ErrorHanding("connnect() error");

    // 接收服务端发送数据直到接收到 EOF 为止，接收到 EOF 时 read 函数会返回 0
    while ((readCnt = recv(sock, buf, BUF_SIZE, 0)) != 0)
        // 将服务端数据写入文件中
        fwrite((void *)buf, 1, readCnt, fp);

    puts("Received file data");
    // 向服务端发送消息，服务端未关闭输入流则可接收
    send(sock, "Thank you", 10, 0);

    fclose(fp);
    close(sock);
    WSACleanup();
    return 0;
}

void ErrorHanding(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}