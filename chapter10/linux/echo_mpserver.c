#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    char buf[BUF_SIZE];
    int str_len, state;

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

    pid_t pid;
    struct sigaction act;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 防止僵尸进程，子进程退出的时候调用 read_childproc 函数
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    // 调用 socket 创建套接字
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    // 初始化地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    // 调用 bind 函数分配 IP 地址和端口号
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    // 调用 listen 函数将套接字转为可连接接收状态
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    // 循环调用 accept 函数，依次向客户端提供服务
    while (1)
    {
        // 每接收一个客户端即创建一个服务端套接字
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            continue;
        else
            puts("new client connected...");

        // 每接收一个客户端创建一个子进程
        // 子进程完全复制父进程的资源，clnt_sock 套接字文件描述符也同时复制了一份
        pid = fork();

        if (pid == -1)
        {
            close(clnt_sock);
            continue;
        }
        if (pid == 0)
        {
            // 子进程运行区域
            close(serv_sock);
            // 子进程提供回声服务
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
                write(clnt_sock, buf, str_len);
            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        else
            // 父进程运行区域
            // clnt_sock 已经复制给子进程了，因此父进程的就可以关闭了
            close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}