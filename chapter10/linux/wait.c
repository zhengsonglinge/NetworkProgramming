#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int status;
    pid_t pid = fork();

    if (pid == 0)
    {
        // 退出第 9 行创建的子进程
        return 3;
    }
    else
    {
        printf("Child PID: %d \n", pid);
        pid = fork();
        if (pid == 0)
        {
            // 退出第 19 行创建的子进程
            exit(7);
        }
        else
        {
            printf("Child PID: %d \n", pid);
            // 之前终止的子进程的相关信息被保存到 status 变量，同时子进程被完全销毁
            wait(&status);
            // 判断子进程是否正常退出并输出子进程返回值
            if (WIFEXITED(status))
                printf("Child send one: %d \n", WEXITSTATUS(status));

            // 因为创建了两个子进程，因此需要两个 wait 函数
            wait(&status);
            if (WIFEXITED(status))
                printf("Child send two: %d \n", WEXITSTATUS(status));

            sleep(30);
        }
    }

    return 0;
}
