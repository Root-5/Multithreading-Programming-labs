#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>

sem_t a, b, c, module;
int widget_count = 0;

void exit_error(int error_num, const char *msg)
{
    char buf[256];
    strerror_r(error_num, buf, sizeof buf);
    fprintf(stderr, "Error: %s\n", buf);
    exit(EXIT_FAILURE);
}

void *a_routine(void *arg)
{
    while (1)
    {
        sleep(1);
        sem_post(&a);
        printf("A was created!\n");
    }
}

void *b_routine(void *arg)
{
    while (1)
    {
        sleep(2);
        sem_post(&b);
        printf("B was created!\n");
    }
}

void *c_routine(void *arg)
{
    while (1)
    {
        sleep(3);
        sem_post(&c);
        printf("C was created!\n");
    }
}

void *module_routine(void *arg)
{
    while (1)
    {
        sem_wait(&b);
        sem_wait(&a);
        sem_post(&module);
        printf("Module was created!\n");
    }
}

void *widget_routine(void *arg)
{
    while (1)
    {
        sem_wait(&c);
        sem_wait(&module);
        widget_count++;
        printf("!!! Widget was created !!!\n");
    }
}

int main(int argc, char *argv[])
{
    pthread_t thread_a, thread_b, thread_c, thread_module, thread_widget;
    int error;
    int a_count = 0, b_count = 0, c_count = 0, module_count = 0;

    error = sem_init(&a, 0, 0);
    if (error)
    {
        exit_error(error, "semaphore initialize failed");
    }
    error = sem_init(&b, 0, 0);
    if (error)
    {
        exit_error(error, "semaphore initialize failed");
    }
    error = sem_init(&c, 0, 0);
    if (error)
    {
        exit_error(error, "semaphore initialize failed");
    }
    error = sem_init(&module, 0, 0);
    if (error)
    {
        exit_error(error, "semaphore initialize failed");
    }

    sigset_t set, oset;
    sigemptyset(&set);
    sigemptyset(&oset);

    sigaddset(&set, SIGINT);
    pthread_sigmask(SIG_BLOCK, &set, &oset);

    error = pthread_create(&thread_a, NULL, a_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed");
    error = pthread_create(&thread_b, NULL, b_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed");
    error = pthread_create(&thread_c, NULL, c_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed");
    error = pthread_create(&thread_module, NULL, module_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed");
    error = pthread_create(&thread_widget, NULL, widget_routine, NULL);
    if (error)
        exit_error(error, "pthread_create failed for thread_a");

    pthread_sigmask(SIG_UNBLOCK, &set, &oset);

    sigwait(&set);

    sem_getvalue(&a, &a_count);
    sem_getvalue(&b, &b_count);
    sem_getvalue(&c, &c_count);
    sem_getvalue(&module, &module_count);

    char buf[256];
    error = pthread_cancel(thread_a);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_cancel(thread_b);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_cancel(thread_c);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_cancel(thread_module);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_cancel(thread_widget);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }

    error = pthread_join(thread_a, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_join(thread_b, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_join(thread_c, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_join(thread_module, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = pthread_join(thread_widget, NULL);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    printf("\nA count: %d\nB count: %d\nC count: %d\nModules count: %d\nWidget count: %d\n", a_count, b_count, c_count, module_count, widget_count);

    //Semaphores destroy
    error = sem_destroy(&a);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = sem_destroy(&b);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = sem_destroy(&c);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }
    error = sem_destroy(&module);
    if (error)
    {
        strerror_r(error, buf, sizeof buf);
        fprintf(stderr, "Error: %s\n", buf);
    }

    pthread_exit(NULL);
    return 0;
}
