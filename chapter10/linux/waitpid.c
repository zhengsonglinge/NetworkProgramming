#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int status;
    pid_t pid = fork();

    if (pid == 0)
    {
        // 子进程延时 15s 退出
        sleep(15);
        return 24;
    }
    else
    {
        // while 循环调用 waitpid 函数，若之前没有终止的子进程则返回 0
        while (!waitpid(-1, &status, WNOHANG))
        {
            sleep(3);
            puts("sleep 3sec...");
        }
        if (WIFEXITED(status))
            printf("Child send %d \n", WEXITSTATUS(status));
    }
    return 0;
}
