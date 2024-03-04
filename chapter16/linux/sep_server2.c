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
    int serv_sock, clnt_sock;
    char buf[BUF_SIZE];

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

    FILE *readfp;
    FILE *writefp;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

    // 将文件描述符转化为 FILE 结构体指针
    // 并调用 dup 函数复制文件描述符
    readfp = fdopen(clnt_sock, "r");
    writefp = fdopen(dup(clnt_sock), "w");

    fputs("FROM SERVER: Hi~ client? \n", writefp);
    fputs("I love all of the world \n", writefp);
    fputs("You are awesome! \n", writefp);
    fflush(writefp);

    // 将 FILE 指针转换为文件描述符并调用 shutdown 函数进行半关闭, 并向客户端发送 EOF
    // 调用 shutdown 函数时，无论复制出多少文件描述符都进入半关闭状态，同时传递 EOF
    shutdown(fileno(writefp), SHUT_WR);
    fclose(writefp);

    // 接收最后的客户端消息
    fgets(buf, sizeof(buf), readfp);
    fputs(buf, stdout);
    fclose(readfp);

    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}