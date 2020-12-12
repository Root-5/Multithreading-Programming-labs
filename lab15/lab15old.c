#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

sem_t *sems[2];

void print(const char *text, int semToPost, int semToWait)
{
    for (int i = 0; i < 10; ++i)
    {
        if (sem_wait(sems[semToWait]))
        {
            fprintf(stderr, "Unable to sem_wait. Error message: %s", strerror(errno));
            exit(1);
        }

        printf("%s\n", text);

        if (sem_post(sems[semToPost]))
        {
            fprintf(stderr, "Unable to sem_post. Error message: %s", strerror(errno));
            exit(1);
        }
    }
}

int main()
{
    if ((sems[0] = sem_open("/mysem1", O_CREAT | O_EXCL, 0777, 1)) == SEM_FAILED)
    {
        fprintf(stderr, "failed on opening 1-th sem. Error message: %s", strerror(errno));
        return 1;
    }
    if ((sems[1] = sem_open("/mysem2", O_CREAT | O_EXCL, 0777, 0)) == SEM_FAILED)
    {
        fprintf(stderr, "failed on opening 2-nd sem. Error message: %s", strerror(errno));
        return 1;
    }

    switch (fork())
    {
    case -1:
        fprintf(stderr, "failed on fork. Error message: %s", strerror(errno));
        return 1;
    case 0:
        print("from primary proc", 0, 1);
        exit(0);
    default:
        print("from main proc", 1, 0);
    }

    if (sem_close(sems[0]) == -1)
    {
        fprintf(stderr, "close 1 failed. Error message: %s", strerror(errno));
    }

    if (sem_close(sems[1]) == -1)
    {
        fprintf(stderr, "close 2 failed. Error message: %s", strerror(errno));
    }

    if (sem_unlink("/mysem1") == -1)
    {
        fprintf(stderr, "unlink 1 failed. Error message: %s", strerror(errno));
    }

    if (sem_unlink("/mysem2") == -1)
    {
        fprintf(stderr, "unlink 2 failed. Error message: %s", strerror(errno));
    }

    return 0;
}
