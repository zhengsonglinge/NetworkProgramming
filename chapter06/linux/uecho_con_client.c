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
    int sock;
    struct sockaddr_in serv_addr, from_addr; // 不再需要 from_addr
    char message[BUF_SIZE];
    int str_len;
    socklen_t addr_size; // 多余变量

    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port> \n", argv[0]);
        exit(1);
    }
    // 使用 SOCK_DGRAM 表示 UDP 协议
    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                 // 地址族（address family）IPv4
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); // 地址
    serv_addr.sin_port = htons(atoi(argv[2]));      // 端口

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1)
    {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        /*
        sendto(sock, message, strlen(message), 0,
               (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        */
        write(sock, message, strlen(message));
        /*
        addr_size = sizeof(from_addr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0,
                           (struct sockaddr *)&from_addr, &addr_size);
        */
        str_len = read(sock, message, sizeof(message) - 1);
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }

    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}