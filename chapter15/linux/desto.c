#include <stdio.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    FILE *fp;
    int fd = open("data.dat", O_WRONLY | O_CREAT | O_TRUNC);

    if (fd == -1)
    {
        fputs("file open error", stdout);
        return -1;
    }

    // 将文件描述符转换为 FILE 指针，w 表示返回写模式的 FILE 指针
    fp = fdopen(fd, "w");
    fputs("Newwork C programming \n", fp);
    // 关闭文件，此时完全关闭，不需要再通过文件描述符关闭。
    // 文件描述符此时也只是一个无意义整数
    fclose(fp);
    return 0;
}
