#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char const *argv[])
{
    int sock;
    FILE *fp;
    struct sockaddr_in serv_addr;
    char buf[BUF_SIZE];
    int read_cnt;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <Port> \n", argv[0]);
        exit(1);
    }

    // 打开待写入文件
    fp = fopen("receive.dat", "wb");

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connnect() error");

    // 接收服务端发送数据直到接收到 EOF 为止，接收到 EOF 时 read 函数会返回 0
    while ((read_cnt = read(sock, buf, BUF_SIZE)) != 0)
        // 将服务端数据写入文件中
        fwrite((void *)buf, 1, read_cnt, fp);

    puts("Received file data");
    // 向服务端发送消息，服务端未关闭输入流则可接收
    write(sock, "Thank you", 10);

    fclose(fp);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}