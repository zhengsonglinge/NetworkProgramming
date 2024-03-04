#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 50
void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    char buf[BUF_SIZE];

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_size;

    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd;

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

    // 创建 epoll 例程
    epfd = epoll_create(EPOLL_SIZE);
    // 动态分配 epoll_event 结构体内存，用来保存发生事件的文件描述符
    ep_events = malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

    // 添加监控文件描述符，用来监控是否有客户端需要连接
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    while (1)
    {
        // 等待事件发生，即等待有数据需要读取
        // -1 表示一直等待到有文件描述符有事件发生为止
        // event_cnt 表示有事件发生的文件描述符个数
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1)
        {
            puts("epoll_wait() error");
            break;
        }

        for (int i = 0; i < event_cnt; i++)
        {
            // 如果文件描述符为监听套接字文件描述符，表示有客户端请求连接
            if (ep_events[i].data.fd == serv_sock)
            {
                addr_size = sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &addr_size);

                // 将与客户端连接的套接字文件描述符添加到 epfd epoll 例程中，用于监控客户端传输数据
                event.events = EPOLLIN;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("connected client: %d \n", clnt_sock);
            }
            else
            // 如果文件描述符为与客户端连接的套接字文件描述符，表示客户端有数据传输
            {
                int str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
                if (str_len == 0)
                {
                    // 当客户端发送 EOF 关闭连接时，删除 epfd epoll 例程中的文件描述符
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    // 关闭文件描述符和套接字
                    close(ep_events[i].data.fd);
                    printf("close client: %d \n", ep_events[i].data.fd);
                }
                else
                {
                    // 回传服务
                    write(ep_events[i].data.fd, buf, str_len);
                }
            }
        }
    }
    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}