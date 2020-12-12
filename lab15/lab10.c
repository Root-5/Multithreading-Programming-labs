#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define NUM_OF_LINES 10
#define MUTEX_NUM 3

pthread_mutex_t mutexes[MUTEX_NUM];
pthread_mutexattr_t mut_attr;
int flag = 0;

void lockMutex(int num)
{
    if (pthread_mutex_lock(mutexes + num))
    {
        printf("Unable to lock mutex");
        pthread_exit((void *)1);
    }
}

void unlockMutex(int num)
{
    if (pthread_mutex_unlock(mutexes + num))
    {
        printf("Unable to unlock mutex");
        pthread_exit((void *)1);
    }
}

void *thread_print(void *arg)
{
    int num = 0;
    lockMutex(2);
    flag = 1;

    for (int i = 0; i < NUM_OF_LINES; ++i)
    {
        lockMutex(num);
        printf("I'm 2nd, line num: %d\n", i + 1);
        unlockMutex((num + 2) % MUTEX_NUM);
        num = (num + 1) % MUTEX_NUM;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thread;

    //init
    pthread_mutexattr_init(&mut_attr);
    pthread_mutexattr_settype(&mut_attr, PTHREAD_MUTEX_ERRORCHECK);

    for (int i = 0; i < MUTEX_NUM; ++i)
    {
        pthread_mutex_init(mutexes + i, &mut_attr);
    }

    lockMutex(0);
    lockMutex(2);
    //thread create & print
    if (pthread_create(&thread, NULL, thread_print, NULL))
    {
        printf("Thread creating failed\n");
        return 1;
    }

    int num = 1;
    unlockMutex(2);
    for (int i = 0; i < NUM_OF_LINES; ++i)
    {
        lockMutex(num);
        printf("I'm 1th, line num: %d\n", i + 1);
        unlockMutex((num + 2) % MUTEX_NUM);
        num = (num + 1) % MUTEX_NUM;
    }

    //join
    if (pthread_join(thread, NULL) != 0)
    {
        printf("Joining failed\n");
        return 1;
    }

    //deinit
    for (int i = 0; i < MUTEX_NUM; ++i)
    {
        if (pthread_mutex_destroy(mutexes + i))
        {
            printf("Unable to destroy mutex");
        }
    }

    return 0;
}
