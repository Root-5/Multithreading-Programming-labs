#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#define MESSAGES_COUNT 500

sem_t q, empty;
int catched = 0; //for signal
int droped;

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

void mymsginit(Queue *)
{
    sem_init(&q, 0, 1);
    sem_init(&empty, 0, 10);
    queue->root = NULL;
    queue->body = NULL;
    droped = 0;
}
void mymsqdrop(Queue *);
void mymsgdestroy(Queue *);
int mymsgput(Queue *, char *msg)
{
    sem_wait(&empty);
    sem_wait(&q);

    if (droped == 0)
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
}
int mymsgget(Queue *, char *buf, size_t bufsize);

int main(int argc, char argv[])
{
}