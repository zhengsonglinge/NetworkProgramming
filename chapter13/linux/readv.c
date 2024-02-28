#include <stdio.h>
#include <sys/uio.h>

#define BUF_SIZE 100

int main(int argc, char const *argv[])
{
    struct iovec vec[2];
    char buf1[BUF_SIZE] = {
        0,
    };
    char buf2[BUF_SIZE] = {
        0,
    };
    int str_len;

    // 设置第一个数据的保存位置和大小，最多保存 5 个字节
    vec[0].iov_base = buf1;
    vec[0].iov_len = 5;
    // 剩余数据保存到 vec[1] 注册的缓冲
    vec[1].iov_base = buf2;
    vec[1].iov_len = BUF_SIZE;

    // 从标准输入中接收数据
    str_len = readv(0, vec, 2);
    printf("Read bytes: %d \n", str_len);
    printf("First message: %s \n", buf1);
    printf("second message: %s \n", buf2);
    return 0;
}
