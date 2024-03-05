#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void *thread_main(void *arg);

int main(int argc, char const *argv[])
{
    pthread_t t_id;
    int thread_param = 5;
    void *thread_return;

    // 创建一个线程去执行 thread_main 函数
    if (pthread_create(&t_id, NULL, thread_main, (void *)&thread_param) != 0)
    {
        puts("pthread_create() error!");
        return -1;
    }

    // 等待 t_id 的线程结束，结束后的返回值保存到 thread_return 变量中
    if (pthread_join(t_id, &thread_return) != 0)
    {
        puts("pthread_join() error!");
        return -1;
    }

    // 输出线程返回值
    printf("Thread return message: %s \n", (char *)thread_return);
    // 返回值是线程执行 thread_main 函数内部动态分配的
    free(thread_return);
    return 0;
}
void *thread_main(void *arg)
{
    int cnt = *((int *)arg);

    char *msg = (char *)malloc(sizeof(char) * 50);
    strcpy(msg, "Hello, I'am thread~ \n");

    for (int i = 0; i < cnt; i++)
    {
        sleep(1);
        puts("running thread");
    }
    return (void *)msg;
}
