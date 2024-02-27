#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// 信号处理函数，这种类型的函数被称为信号处理器（Handler）
void timeout(int sig)
{
    if (sig == SIGALRM)
        puts("Time out!");
    alarm(2);
}

int main(int argc, char const *argv[])
{

    struct sigaction act;
    // 注册信号处理函数，处理函数指针赋值给 sigaction 字段的 sa_handler 字段
    act.sa_handler = timeout;
    // sigaction 结构体的 sa_mask 字段所有位初始化为 0
    sigemptyset(&act.sa_mask);
    // sa_falgs 字段也初始化为 0
    act.sa_flags = 0;

    // 注册 SIGALRM 信号的处理器
    sigaction(SIGALRM, &act, 0);

    // 2s 后发送 SIGALRM 信号
    alarm(2);

    for (int i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(100);
    }
    return 0;
}
