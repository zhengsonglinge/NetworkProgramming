#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 30

void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int send_sock;
    struct sockaddr_in mul_addr;
    int time_live = TTL;
    FILE *fp;
    char buf[BUF_SIZE];

    if (argc != 3)
    {
        printf("Usage: %s <GroupIP> <Port> \n", argv[0]);
        exit(1);
    }

    // 多播通过 UDP 实现的，因此创建 UDP 套接字
    send_sock = socket(PF_INET, SOCK_DGRAM, 0);

    // 设置传输数据的目标地址信息，必须将 IP 地址设置为多播地址
    memset(&mul_addr, 0, sizeof(mul_addr));
    mul_addr.sin_family = AF_INET;
    mul_addr.sin_addr.s_addr = inet_addr(argv[1]); // 多播 IP
    mul_addr.sin_port = htons(atoi(argv[2]));      // 多播端口

    // 指定套接字 TTL 信息
    setsockopt(send_sock, IPPROTO_IP,
               IP_MULTICAST_TTL, (void *)&time_live, sizeof(time_live));

    if ((fp = fopen("news_sender.c", "r")) == NULL)
        error_handling("fopen() error");

    // 读取到 EOF 终止，即读取到文件结束
    while (!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        // 基于 UDP 传输数据，因此使用 sendto 函数传输数据
        sendto(send_sock, buf, strlen(buf),
               0, (struct sockaddr *)&mul_addr, sizeof(mul_addr));
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
