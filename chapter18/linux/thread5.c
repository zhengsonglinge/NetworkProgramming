#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define NUM_THREAD 100
void *thread_inc(void *arg);
void *thread_des(void *arg);
long long num = 0;

// 声明保存互斥量读取值的变量，全局变量供三个函数使用
pthread_mutex_t mutex;

int main(int argc, char const *argv[])
{
    pthread_t thread_id[NUM_THREAD];

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_THREAD; i++)
    {
        if (i % 2 == 0)
            pthread_create(&thread_id[i], NULL, thread_inc, NULL);
        else
            pthread_create(&thread_id[i], NULL, thread_des, NULL);
    }

    for (int i = 0; i < NUM_THREAD; i++)
        pthread_join(thread_id[i], NULL);

    printf("result: %lld \n", num);
    // 销毁互斥量
    pthread_mutex_destroy(&mutex);
    return 0;
}

void *thread_inc(void *arg)
{
    // 临界区只有 num++ 一行，但循环语句一起被锁了
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 50000000; i++)
        num++;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *thread_des(void *arg)
{
    for (int i = 0; i < 50000000; i++)
    {
        // 锁住临界区
        pthread_mutex_lock(&mutex);
        num--;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}