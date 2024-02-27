#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char const *argv[])
{
    // fds 数组中保存用于 I/O 的文件描述符
    int fds[2];
    char str[] = "Who are you?";
    char buf[BUF_SIZE];
    __pid_t pid;

    // pipe 函数创建管道
    pipe(fds);

    // fork 函数复制的并非管道，而是用于管道 I/O 的文件描述符
    // 父子进程同时拥有 I/O 文件描述符
    pid = fork();

    if (pid == 0)
    {
        write(fds[1], str, sizeof(str));
    }
    else
    {
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }
    return 0;
}

/*
Who are you?
*/
