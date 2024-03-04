#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char const *argv[])
{
    int sock;
    pid_t pid;
    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];
    int str_len;

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

    pid = fork();

    if (pid == 0)
        // 子进程写数据
        write_routine(sock, buf);
    else
        // 父进程读数据
        read_routine(sock, buf);

    close(sock);
    return 0;
}

void read_routine(int sock, char *buf)
{
    while (1)
    {
        int str_len = read(sock, buf, BUF_SIZE);
        if (str_len == 0)
            return;
        buf[str_len] = 0;
        printf("Message from server: %s", buf);
    }
}
void write_routine(int sock, char *buf)
{
    while (1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n"))
        {
            // 因为通过 fork 函数复制了文件描述符，
            // 因此子进程执行完毕 write_routine 之后调用 main 最后的 close(sock) 一次是不能关闭套接字的
            // 父进程中的 read_routine 不会结束，因此无法调用 close(sock)
            // 只能另外使用 shutdown 函数调用指定关闭 sock 套接字
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}