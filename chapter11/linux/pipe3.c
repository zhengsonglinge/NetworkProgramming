#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char const *argv[])
{
    int fds1[2], fds2[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    __pid_t pid;

    // 创建两个管道
    pipe(fds1); // fds1 管道用于子进程向父进程传输数据
    pipe(fds2); // fds2 管道用于父进程向子进程传输数据

    pid = fork();

    if (pid == 0)
    {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    else
    {
        read(fds1[0], buf, BUF_SIZE);
        printf("Parent proc output: %s \n", buf);
        write(fds2[1], str2, sizeof(str2));
        // 没有太大意义，延迟父进程终止，防止父进程先于子进程结束
        sleep(1);
    }
    return 0;
}