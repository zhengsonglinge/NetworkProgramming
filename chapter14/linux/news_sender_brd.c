#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30

void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int send_sock;
    struct sockaddr_in broad_addr;
    FILE *fp;
    char buf[BUF_SIZE];
    int so_brd = 1;

    if (argc != 3)
    {
        printf("Usage: %s <Broadcase IP> <Port> \n", argv[0]);
        exit(1);
    }

    // 广播通过 UDP 实现的，因此创建 UDP 套接字
    send_sock = socket(PF_INET, SOCK_DGRAM, 0);

    memset(&broad_addr, 0, sizeof(broad_addr));
    broad_addr.sin_family = AF_INET;
    broad_addr.sin_addr.s_addr = inet_addr(argv[1]); // 广播 IP
    broad_addr.sin_port = htons(atoi(argv[2]));      // 广播端口

    // 设置广播选项
    setsockopt(send_sock, SOL_SOCKET,
               SO_BROADCAST, (void *)&so_brd, sizeof(so_brd));

    if ((fp = fopen("news_sender_brd.c", "r")) == NULL)
        error_handling("fopen() error");

    // 读取到 EOF 终止，即读取到文件结束
    while (!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        // 基于 UDP 传输数据，因此使用 sendto 函数传输数据
        sendto(send_sock, buf, strlen(buf),
               0, (struct sockaddr *)&broad_addr, sizeof(broad_addr));
        sleep(2);
    }
    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
