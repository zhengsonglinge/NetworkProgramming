#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    int cfd1, cfd2;
    char str1[] = "Hi~ \n";
    char str2[] = "It's nice day~ \n";

    // 复制文件描述符 1，cfd2 指定文件描述符整数值
    cfd1 = dup(1);
    cfd2 = dup2(cfd1, 7);

    printf("fd1=%d, fd2=%d \n", cfd1, cfd2);
    write(cfd1, str1, sizeof(str1));
    write(cfd2, str2, sizeof(str2));

    close(cfd1);
    close(cfd2);

    // 仍有一个文件描述符用于输出到控制台
    write(1, str1, sizeof(str1));

    // 关闭之后无法再输出字符串到控制台了
    close(1);
    write(1, str2, sizeof(str2));
    return 0;
}
