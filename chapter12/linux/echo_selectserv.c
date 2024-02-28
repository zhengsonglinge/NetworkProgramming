#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    char buf[BUF_SIZE];
    int str_len, state;

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_size;

    fd_set reads, cpy_reads;
    struct timeval timeout;
    int fd_num;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    // 注册服务端套接字到 fd_set 中，如果服务端套接字有变化说明有客户端请求连接
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    int fd_max = serv_sock;

    while (1)
    {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        // 只监控是否有待读取数据，其他不需要监控置为 0
        // 因为文件描述符是从 0 开始的，因此这里文件描述符的数量是 fd_max+1
        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;

        // 超时，此时没有文件描述符发生变化，重新调用 select 并等待
        if (fd_num == 0)
            continue;

        // 当 fd_num > 0 时说明有文件描述符发生变化，循环逐个检查
        for (int i = 0; i < fd_max + 1; i++)
        {
            // 第 i 个文件描述符是否发生变化
            if (FD_ISSET(i, &cpy_reads))
            {
                // 如果文件描述符是服务端套接字的文件描述符，则说明有客户端请求连接
                if (i == serv_sock)
                {
                    addr_size = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);
                    // 注册与客户端连接的套接字的文件描述符
                    FD_SET(clnt_sock, &reads);
                    if (fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    printf("connected client: %d \n", clnt_sock);
                }
                // 如果不是服务端套接字，说明是与客户端连接的套接字需要接收数据
                else
                {
                    str_len = read(i, buf, BUF_SIZE);
                    // str_len 为 0，表示客户端发来 EOF 要断开连接
                    if (str_len == 0)
                    {
                        // 注销注册过的文件描述符
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d \n", i);
                    }
                    else
                    {
                        // 执行回声服务
                        write(i, buf, str_len);
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}