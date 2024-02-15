#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

#define BUF_SIZE 1024 // 字符串字节数
#define RLT_SIZE 4    // 结果字节数
#define OPSZ 4        // 运算值字节数

int main(int argc, char const *argv[])
{
    int sock;
    // 待运算数字个数 + 待运算数字 + 运算符
    char opmsg[BUF_SIZE];
    int result, opnd_cnt, i;
    struct sockaddr_in serv_adr;

    if (argc != 3)
    {
        printf("Ussage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error!");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("connect() error!");
    else
        puts("Connected.....");

    fputs("Operand count: ", stdout);
    scanf("%d", &opnd_cnt);
    // 待运算数个数，用 1 字节表示，并转换为 char 类型存储到 opmsg 中
    opmsg[0] = (char)opnd_cnt;

    // 逐个输入待运算数并保存到 opmsg 数组中，每个待运算数 4 个字节
    for (i = 0; i < opnd_cnt; i++)
    {
        printf("Operand %d: ", i + 1);
        scanf("%d", (int *)&opmsg[i * OPSZ + 1]); // +1 是因为 0 位存储了待运算数字个数
    }

    // 删掉缓冲区中的字符 \n
    fgetc(stdin);
    fputs("Operator: ", stdout);
    // 输入运算符并保存到 opmsg 数组中
    scanf("%c", &opmsg[opnd_cnt * OPSZ + 1]);
    // 调用 write 函数传输 opmsg 数组中的内容，可以调用一次，也可以调用多次 (TCP 不存在数据边界)
    write(sock, opmsg, opnd_cnt * OPSZ + 2);
    // 保存服务端计算结果，待接收长度 4 字节，因此可以调用一次 read 函数
    read(sock, &result, RLT_SIZE);

    printf("Operation result: %d\n", result);
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}