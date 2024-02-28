#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <signal.h>

#define BUF_SIZE 30
void error_handling(char *message);
void urg_handler(int signo);

int serv_sock, clnt_sock;

int main(int argc, char const *argv[])
{
    char buf[BUF_SIZE];
    int str_len, state;

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_size;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    struct sigaction act;
    act.sa_handler = urg_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);

    fcntl(clnt_sock, __F_SETOWN, getpid());
    // 注册信号函数，当操作系统向线程发送 SIGURG 信号的时候调用 urg_handler 函数
    state = sigaction(SIGURG, &act, 0);

    while ((str_len = recv(clnt_sock, buf, sizeof(buf), 0)) != 0)
    {
        if (str_len == -1)
            continue;
        buf[str_len] = 0;
        puts(buf);
    }

    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void urg_handler(int signo)
{
    int str_len;
    char buf[BUF_SIZE];
    // 使用 recv 函数接收紧急消息
    str_len = recv(clnt_sock, buf, sizeof(buf) - 1, MSG_OOB);
    buf[str_len] = 0;
    printf("Urgent message: %s \n", buf);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}