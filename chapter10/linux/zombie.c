#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    __pid_t pid = fork();

    if (pid == 0)
    {
        // Child Progress
        puts("Hi,I am a Child process");
    }
    else
    {
        // Parent Progress
        printf("Child Process ID: %d \n", pid);
        sleep(30);
    }

    if (pid == 0)
        // Child Progress
        puts("End child process");
    else
        // Parent Progress
        puts("End parent process");

    return 0;
}
