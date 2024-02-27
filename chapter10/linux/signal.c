#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// 信号处理函数，这种类型的函数被称为信号处理器（Handler）
void timeout(int sig)
{
    if (sig == SIGALRM)
        puts("Time out!");
    // 每隔 2s 重复产生 SIGALRM 信号
    alarm(2);
}

// 信号处理函数，这种类型的函数被称为信号处理器（Handler）
void keycontrol(int sig)
{
    if (sig == SIGINT)
        puts("CTRL+C pressed");
}

int main(int argc, char const *argv[])
{
    // 注册 SIGALRM 信号处理器
    signal(SIGALRM, timeout);
    // 注册 SIGINT 信号处理器
    signal(SIGINT, keycontrol);
    // 2s 后发出 SIGALRM 信号
    alarm(2);

    for (int i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(100);
    }
    return 0;
}
