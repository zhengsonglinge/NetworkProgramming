#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig)
{
    int status;
    // waitpid 程序保证子进程不会称为僵尸进程
    pid_t id = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status))
    {
        printf("Removed proc id: %d \n", id);
        printf("Child send: %d \n", WEXITSTATUS(status));
    }
}

int main(int argc, char const *argv[])
{
    pid_t pid;
    struct sigaction act;
    // 注册信号处理函数，如果有 SIGCHLD 信号则调用 read_childproc 函数
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    // 创建第一个子进程，通过 return 12 终止子进程
    pid = fork();

    if (pid == 0)
    {
        // 子进程执行
        puts("Hi! I'm child process");
        sleep(10);
        return 12;
    }
    else
    {
        // 父进程执行
        printf("Child proc id: %d\n", pid);

        // 创建第二个子进程，子进程通过 exit(24) 终止子进程
        pid = fork();

        if (pid == 0)
        {
            // 子进程执行
            puts("Hi! I'm child process");
            sleep(10);
            exit(24);
        }
        else
        {
            // 父进程执行
            printf("Child proc id: %d\n", pid);
            for (int i = 0; i < 5; i++)
            {
                puts("wait...");
                sleep(5);
            }
        }
    }
    return 0;
}
