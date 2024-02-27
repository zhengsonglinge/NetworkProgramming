#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char const *argv[])
{
    // fds 数组中保存用于 I/O 的文件描述符
    int fds[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    __pid_t pid;

    // pipe 函数创建管道
    pipe(fds);

    // fork 函数复制的并非管道，而是用于管道 I/O 的文件描述符
    // 父子进程同时拥有 I/O 文件描述符
    pid = fork();

    if (pid == 0)
    {
        write(fds[1], str1, sizeof(str1));
        sleep(2);
        read(fds[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    else
    {
        read(fds[0], buf, BUF_SIZE);
        printf("Parent proc output: %s \n", buf);
        write(fds[1], str2, sizeof(str2));
        sleep(3); // 防止父进程先于子进程退出
    }
    return 0;
}