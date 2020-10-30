#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

char *strings[4][4] = {{"String 1 from thread 1", "String 2 from thread 1", "String 3 from thread 1", "String 4 from thread 1"},
                       {"String 1 from thread 2", "String 2 from thread 2", "String 3 from thread 2", "String 4 from thread 2"},
                       {"String 1 from thread 3", "String 2 from thread 3", "String 3 from thread 3", "String 4 from thread 3"},
                       {"String 1 from thread 4", "String 2 from thread 4", "String 3 from thread 4", "String 4 from thread 4"}};

void print(void *strings)
{
    char **strings_tmp = (char **)strings;
    int i = 0;
    for (; i < 4; ++i)
    {
        printf("%s\n", strings_tmp[i]);
    }
}

int main(int argc, char *argv[])
{
    pthread_t threads[4];
    int code = 0;
    int i = 0;
    for (; i < 4; ++i)
    {
        code = pthread_create(&threads[i], NULL, (void *)print, (void *)strings[i]);
        if (code != 0)
        {
            char buf[256];
            strerror_r(code, buf, (size_t)(sizeof buf));
            fprintf(stderr, "%s: thread creating error: %s", argv[0], buf);
            exit(1);
        }
    }
    pthread_exit(0);
}