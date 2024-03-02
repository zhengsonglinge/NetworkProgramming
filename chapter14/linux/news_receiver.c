#define _GNU_SOURCE

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
    int recv_sock;
    struct sockaddr_in addr;
    struct ip_mreq join_addr;
    char buf[BUF_SIZE];

    if (argc != 3)
    {
        printf("Usage: %s <GroupIP> <Port> \n", argv[0]);
        exit(1);
    }

    recv_sock = socket(PF_INET, SOCK_DGRAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[2]));

    if (bind(recv_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        error_handling("bind() error");

    // 初始化 ip_mreq 变量
    join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]); // 初始化多播地址
    join_addr.imr_interface.s_addr = htonl(INADDR_ANY);  // 初始化待加入主机的 IP 地址

    // 利用套接字选项 IP_ADD_MEMBERSHIP 加入多播组
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
               (void *)&join_addr, sizeof(join_addr));

    while (1)
    {
        // 接收多播数据，最后两个参数 NULL 和 0 表示不需要知道发送方地址
        int str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        if (str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }
    close(recv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
