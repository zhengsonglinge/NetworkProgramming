#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void error_handling(char *message);
void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

// 用于管理接入客户端套接字的变量和数组，访问这两个变量的代码构成临界区
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutex;

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    char message[BUF_SIZE];
    int str_len;
    pthread_t t_id;

    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

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

    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

        // 每当有新连接，相关信息写入变量 clnt_cnt 和 clnt_socks
        pthread_mutex_lock(&mutex);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutex);

        // 创建新线程向新接入的客户端提供服务，执行 handle_clnt 函数
        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        // 从内存中完全销毁已终止的线程
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_addr.sin_addr));
    }

    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUF_SIZE];

    // 将接收到的数据发送给全部客户端
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    // 发送消息完毕后删除 clnt_socks 数组中的套接字文件描述符
    // 访问 clnt_socks 数组的代码是临界区
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (clnt_sock == clnt_socks[i])
        {
            while (i++ < clnt_cnt - 1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutex);
    close(clnt_sock);
}
// 将消息发送给所有已经连接的客户端
void send_msg(char *msg, int len)
{
    // 访问 clnt_socks 数组的代码是临界区
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len);
    pthread_mutex_unlock(&mutex);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}