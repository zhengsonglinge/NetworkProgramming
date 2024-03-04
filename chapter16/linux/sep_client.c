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
    char buf[BUF_SIZE];
    int str_len;

    FILE *readfp;
    FILE *writefp;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port> \n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connnect() error");
    else
        puts("Connected.......");

    // 创建读模式和写模式的 FILE 指针
    readfp = fdopen(sock, "r");
    writefp = fdopen(sock, "w");
    while (1)
    {
        // 读到 EOF 时返回 NULL
        if (fgets(buf, sizeof(buf), readfp) == NULL)
            break;
        fputs(buf, stdout);
        fflush(stdout);
    }

    // 向服务器发送最后字符串
    fputs("FROM CLIENT: thank you! \n", writefp);
    fflush(writefp);

    fclose(writefp);
    fclose(readfp);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}