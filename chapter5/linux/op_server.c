#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4
void error_handling(char *message);
int calculate(int opnum, int opnds[], char oprator);

int main(int argc, char const *argv[])
{
    int serv_sock, clnt_sock;
    char opinfo[BUF_SIZE];
    int result, opnd_cnt, i;
    int recv_cnt, recv_len;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 调用 socket 创建套接字
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;                // 地址族（address family）IPv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 地址,INADDR_ANY 表示接受来自任何网络接口的连接
    serv_addr.sin_port = htons(atoi(argv[1]));     // 端口

    // 调用 bind 函数分配 IP 地址和端口号
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    // 调用 listen 函数将套接字转为可连接接收状态
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    clnt_addr_size = sizeof(clnt_addr);
    // 循环调用 accept 函数，依次向客户端提供服务
    for (i = 0; i < 5; i++)
    {
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");

        // 读取第一个字节的待计算数个数
        opnd_cnt = 0;
        read(clnt_sock, &opnd_cnt, 1);

        // 将剩余的待运算数和运算符都读取到 opinfo 数组中，读取的大小为 opnd_cnt*OPSZ + 1
        recv_len = 0;
        while ((opnd_cnt * OPSZ + 1) > recv_len)
        {
            recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE - 1);
            recv_len += recv_cnt;
        }

        // 计算结果
        result = calculate(opnd_cnt, (int *)opinfo, opinfo[recv_len - 1]);
        // 将结果写回客户端，4 字节
        write(clnt_sock, (char *)&result, sizeof(result));
        close(clnt_sock);
    }

    // 向5个客户端提供服务后关闭服务器端套接字并终止程序。
    close(serv_sock);
    return 0;
}

int calculate(int opnum, int opnds[], char op)
{
    int result = opnds[0], i;
    switch (op)
    {
    case '+':
        for (i = 1; i < opnum; i++)
            result += opnds[i];
        break;
    case '-':
        for (i = 1; i < opnum; i++)
            result -= opnds[i];
        break;
    case '*':
        for (i = 1; i < opnum; i++)
            result *= opnds[i];
        break;
    }
    return result;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
