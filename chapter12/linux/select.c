#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char const *argv[])
{
    fd_set reads, temps;
    char buf[BUF_SIZE];
    struct timeval timeout;

    // 初始化 fd_set 变量
    FD_ZERO(&reads);
    // 将文件描述符 0 对应的位置设为 1，0 代表标准输入，因此是监控标准输入的变化
    FD_SET(0, &reads);

    /*
    这是为了设置 select 函数的超时而添加的。但不能在此时设置超时。
    因为调用select函数后，结构体 timeval 的成员 tv-sec 和 tv_usec 的值将被替换为超时前剩余时间。
    因此，调用select函数前，每次都需要初始化 timeval 结构体变量。
    timeout.tv_sec = 5;
    timeout.tv_usec = 5000;
    */

    while (1)
    {
        // 调用 select 函数之后，除发生变化的文件描述符对应位之外，剩下的所有位初始化为 0
        // 因此需要记住初始值，因此用 temp 变量进行计算，而初始值 reads 不变，重复复制过程
        temps = reads;

        // 每次调用 select 前都初始化 timeval 的变量
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // 调用 select 函数
        int result = select(1, &temps, 0, 0, &timeout);

        if (result == -1)
        {
            puts("select() error!");
            break;
        }
        else if (result == 0)
        {
            puts("Time-out!");
        }
        else
        {
            // 验证发生变化的文件描述符是否是标准输入
            // 若是标准输入则向控制台输出
            if (FD_ISSET(0, &temps))
            {
                int str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }
    return 0;
}
