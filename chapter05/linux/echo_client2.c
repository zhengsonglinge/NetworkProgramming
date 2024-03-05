#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUF_SIZE];
    int str_len, recv_len, recv_cnt;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port> \n", argv[0]);
        exit(1);
    }
    // 创建套接字，但此时套接字并不马上分为服务器端和客户端。
    // 如果紧接着调用 bind、listen 函数，将成为服务器端套接字；
    // 如果调用connect函数，将成为客户端套接字。
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                 // 地址族（address family）IPv4
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); // 地址
    serv_addr.sin_port = htons(atoi(argv[2]));      // 端口

    // 调用connect函数向服务器端发送连接请求。
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connnect() error");
    else
        puts("Connected.......");

    while (1)
    {
        // 从命令行读取字符串到 message 中
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);

        // 输入 q 或 Q 退出程序
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        str_len = write(sock, message, strlen(message));

        // 循环调用 read 函数直到接收到的数据量大于字符串长度
        recv_len = 0;
        while (recv_len < str_len)
        {
            recv_cnt = read(sock, message, BUFSIZ - 1);
            if (recv_cnt == -1)
                error_handling("read() error!");
            recv_len += recv_cnt;
        }
        message[recv_len] = 0;
        printf("Message from server : %s", message);
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