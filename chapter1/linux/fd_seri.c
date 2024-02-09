#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

int main(int argc, char const *argv[])
{
    int fd1, fd2, fd3;
    fd1 = socket(PF_INET, SOCK_STREAM, 0);
    fd2 = open("fd_seri.c", SOCK_DGRAM, O_CREAT | O_WRONLY | O_TRUNC);
    fd3 = socket(PF_INET, SOCK_STREAM, 0);

    printf("file desctriptor 1: %d \n", fd1);
    printf("file desctriptor 2: %d \n", fd2);
    printf("file desctriptor 3: %d \n", fd3);

    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}