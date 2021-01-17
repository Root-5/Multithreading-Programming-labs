#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#define MESSAGES_COUNT 500

sem_t empty, fill, q;
int droped, catched = 0;

typedef struct message
{
    char message[81];
    struct message *next;
    struct message *prev;
} Message;

typedef struct queue
{
    Message *root;
    Message *body;
} Queue;

void error_exit(int error_num)
{
    char buf[256];
    strerror_r(error_num, buf, sizeof(buf));
    fprintf(stderr, buf);
    exit(EXIT_FAILURE);
}

void mymsginit(Queue *queue)
{
    sem_init(&q, 0, 1);
    sem_init(&fill, 0, 0);
    sem_init(&empty, 0, 10);
    queue->root = NULL;
    queue->body = NULL;
    droped = 0;
}

void mymsgdrop(Queue *queue)
{
    sem_wait(&q);
    droped = 1;
    sem_post(&empty);
    sem_post(&fill);
    sem_post(&q);
}

void mymsgdestroy(Queue *queue)
{
    sem_wait(&q);
    Message *temp = queue->root, *temp1;
    while (temp != NULL)
    {
        temp1 = temp;
        temp = temp->next;
        free(temp1);
    }

    sem_post(&q);
    sem_destroy(&q);
    sem_destroy(&empty);
    sem_destroy(&fill);
}

int mymsgput(Queue *queue, char *msg)
{
    if (!queue)
        return 0;
    sem_wait(&empty);
    sem_wait(&q);

    if (droped == 1)
    {
        sem_post(&q);
        sem_post(&empty);
        return 0;
    }

    Message *new_msg;
    new_msg = (Message *)malloc(sizeof(Message));
    new_msg->prev = NULL;
    new_msg->next = NULL;

    Message *tmp;
    sprintf(new_msg->message, "%s", "");
    strncat(new_msg->message, msg, 80);

    tmp = queue->root;
    queue->root = new_msg;
    if (tmp)
    {
        (queue->root)->next = tmp;
        tmp->prev = queue->root;
    }

    if (!tmp)
    {
        queue->body = new_msg;
    }
    sem_post(&q);
    sem_post(&fill);
    if (strlen(msg) > 80)
    {
        return 80;
    }
    else
    {
        return strlen(msg);
    }
}

int mymsgget(Queue *queue, char *buf, size_t bufSize)
{
    if (!queue)
        return 0;
    sem_wait(&fill);
    sem_wait(&q);

    if (droped == 1)
    {
        sem_post(&q);
        sem_post(&fill);
        return 0;
    }

    Message *res = queue->body;
    if (queue->root == queue->body)
    {
        queue->root = NULL;
        queue->body = NULL;
    }
    else
    {
        queue->body = queue->body->prev;
        queue->body->next = NULL;
    }
    strncpy(buf, res->message, bufSize);
    free(res);
    sem_post(&q);
    sem_post(&empty);
    return bufSize;
}

void *producer(void *arg)
{
    Queue *queue = (Queue *)arg;
    int i = 0;
    while (1)
    {
        if (catched == 1)
            break;
        char buf[40];
        sprintf(buf, "Message %d from thread %d", i, pthread_self());
        if (mymsgput(queue, buf) == 0)
            pthread_exit(NULL);
        i++;

    }
    pthread_exit(NULL);
}

void *consumer(void *arg)
{
    Queue *queue = (Queue *)arg;
    int length;
    while (1)
    {
        if (catched == 1)
            break;
        char buf[41];
        length = mymsgget(queue, buf, sizeof(buf));
        if (length == 0)
        {
            break;
        }
        else
        {
            printf("Received by thread %d: %s\n", pthread_self(), buf);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    Queue queue;
    int count_producers, count_consumers, error_num;
    pthread_t *producers, *consumers;

    if (argc != 3)
    {
        perror("Incorrect args\n");
        exit(EXIT_FAILURE);
    }

    count_producers = atoi(argv[1]);
    count_consumers = atoi(argv[2]);

    if (count_producers == 0 || count_consumers == 0)
    {
        perror("Incorrect args\n");
        exit(EXIT_FAILURE);
    }
    producers = (pthread_t *)malloc(sizeof(pthread_t *) * count_producers);
    consumers = (pthread_t *)malloc(sizeof(pthread_t *) * count_consumers);

    if (!producers || !consumers)
    {
        perror("Fail. Can't allocate memory for threads\n");
        exit(EXIT_FAILURE);
    }

    mymsginit(&queue);

    sigset_t sig_set;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGINT);

    pthread_sigmask(SIG_BLOCK, &sig_set, NULL);
    int i;
    for (i = 0; i < count_producers || i < count_consumers; i++)
    {
        if (i < count_producers)
        {
            error_num = pthread_create(&producers[i], NULL, producer, &queue);
            if (error_num != 0)
                error_exit(error_num);
        }
        if (i < count_consumers)
        {
            error_num = pthread_create(&consumers[i], NULL, consumer, &queue);
            if (error_num != 0)
                error_exit(error_num);
        }
    }
    pthread_sigmask(SIG_UNBLOCK, &sig_set, NULL);
    if (sigwait(&sig_set) == SIGINT)
        catched = 1;

    mymsgdrop(&queue);

    for (i = 0; i < count_producers || i < count_consumers; i++)
    {
        if (i < count_producers)
        {
            error_num = pthread_join(producers[i], NULL);
            if (error_num != 0)
                error_exit(error_num);
        }
        if (i < count_consumers)
        {
            error_num = pthread_join(consumers[i], NULL);
            if (error_num != 0)
                error_exit(error_num);
        }
    }

    mymsgdestroy(&queue);

    printf("\nAll threads quit and queue destroyed\n");

    free(producers);
    free(consumers);

    pthread_exit(NULL);
    return (EXIT_SUCCESS);
}