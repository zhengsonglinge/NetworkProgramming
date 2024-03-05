#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void *read(void *arg);
void *accu(void *arg);

static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char const *argv[])
{
    pthread_t id_t1, id_t2;

    // 生成两个信号量，一个为0，一个为1
    sem_init(&sem_one, 0, 0);
    sem_init(&sem_two, 0, 1);

    pthread_create(&id_t1, NULL, read, NULL);
    pthread_create(&id_t2, NULL, accu, NULL);

    pthread_join(id_t1, NULL);
    pthread_join(id_t2, NULL);

    sem_destroy(&sem_one);
    sem_destroy(&sem_two);

    return 0;
}

void *read(void *arg)
{
    int sum = 0;
    for (int i = 0; i < 5; i++)
    {
        fputs("Input num: ", stdout);

        // sem_one 和 sem_two 的作用是协调线程调用 read 和 accu 函数的顺序
        // 先调用 read 函数，再调用 accu 函数
        // 防止重复调用 read 覆盖之前值
        sem_wait(&sem_two);
        scanf("%d", &num);
        sem_post(&sem_one);
    }
}

void *accu(void *arg)
{
    int sum = 0;
    for (int i = 0; i < 5; i++)
    {
        // 防止重复计算值
        sem_wait(&sem_one);
        sum += num;
        sem_post(&sem_two);
    }
    printf("Result: %d \n", sum);
}
