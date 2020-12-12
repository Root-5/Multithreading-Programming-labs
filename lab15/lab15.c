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
    switch (fork())
    {
    case -1:
        perror("failed on fork");
        return 1;
    case 0:
        if ((sems[0] = sem_open("/mysemap1", O_CREAT)) == SEM_FAILED)
        {
            perror("failed on opening 1-th sem.");
        }
        if ((sems[1] = sem_open("/mysemap2", O_CREAT)) == SEM_FAILED)
        {
            perror("failed on opening 2-nd sem.");
        }
        print("from primary proc", 0, 1);
        exit(0);
    default:
        if ((sems[0] = sem_open("/mysemap1", O_CREAT | O_EXCL, 0660, 1)) == SEM_FAILED)
        {
            perror("failed on opening 1-th sem.");
        }
        if ((sems[1] = sem_open("/mysemap2", O_CREAT | O_EXCL, 0660, 0)) == SEM_FAILED)
        {
            perror("failed on opening 2-nd sem.");
        }
        print("from main proc", 1, 0);
    }

    if (sem_close(sems[0]) == -1)
    {
        perror("close 1 failed.");
    }

    if (sem_close(sems[1]) == -1)
    {
        perror("close 2 failed.");
    }

    if (sem_unlink("/mysemap1") == -1)
    {
        perror("unlink 1 failed.");
    }

    if (sem_unlink("/mysemap2") == -1)
    {
        perror("unlink 2 failed.");
    }

    return 0;
}