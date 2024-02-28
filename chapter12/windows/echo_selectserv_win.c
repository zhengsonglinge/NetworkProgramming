#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char const *argv[])
{
    WSADATA wsaData;
    SOCKET servSock, clntSock;
    char buf[BUF_SIZE];
    int strLen;

    struct sockaddr_in servAddr, clntAddr;
    int addrSize;

    fd_set reads, cpyReads;
    TIMEVAL timeout;
    int fdNum;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    servSock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    if (listen(servSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    // 注册服务端套接字到 fd_set 中，如果服务端套接字有变化说明有客户端请求连接
    FD_ZERO(&reads);
    FD_SET(servSock, &reads);

    while (1)
    {
        cpyReads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // 只监控是否有待读取数据，其他不需要监控置为 0
        // 因为文件描述符是从 0 开始的，因此这里文件描述符的数量是 fd_max+1
        if ((fdNum = select(1, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
            break;

        // 超时，此时没有文件描述符发生变化，重新调用 select 并等待
        if (fdNum == 0)
            continue;

        // 当 fdNum > 0 时说明有文件描述符发生变化，循环逐个检查
        for (int i = 0; i < reads.fd_count; i++)
        {
            // 第 i 个文件描述符是否发生变化
            if (FD_ISSET(reads.fd_array[i], &cpyReads))
            {
                // 如果文件描述符是服务端套接字的文件描述符，则说明有客户端请求连接
                if (reads.fd_array[i] == servSock)
                {
                    addrSize = sizeof(clntAddr);
                    clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &addrSize);
                    // 注册与客户端连接的套接字的文件描述符
                    FD_SET(clntSock, &reads);
                    printf("connected client: %d \n", clntSock);
                }
                // 如果不是服务端套接字，说明是与客户端连接的套接字需要接收数据
                else
                {
                    strLen = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
                    // strLen 为 0，表示客户端发来 EOF 要断开连接
                    if (strLen == 0)
                    {
                        // 注销注册过的文件描述符
                        FD_CLR(reads.fd_array[i], &reads);
                        closesocket(cpyReads.fd_array[i]);
                        printf("closed client: %d \n", cpyReads.fd_array[i]);
                    }
                    else
                    {
                        // 执行回声服务
                        send(reads.fd_array[i], buf, strLen, 0);
                    }
                }
            }
        }
    }
    closesocket(servSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}