#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char const *argv[])
{
    struct iovec vec[2];
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567";
    int str_len;

    // 第一个传输数据的保存位置和大小
    vec[0].iov_base = buf1;
    vec[0].iov_len = 3;
    // 第二个传输数据的保存位置和大小
    vec[1].iov_base = buf2;
    vec[1].iov_len = 4;

    // 1 表示向标准输出写入数据
    str_len = writev(1, vec, 2);
    puts("");
    printf("Write bytes: %d \n", str_len);
    return 0;
}
