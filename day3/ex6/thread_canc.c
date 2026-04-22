#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void *my_thread(void *parm) {
    printf("thread: entered\n");
    while (1) {
        printf("thread: looping\n");
        pthread_testcancel();
        sleep(1);
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t thread;
    int rc;
    void *status;

    printf("main enter\n");

    printf("thread: create\n");
    rc = pthread_create(&thread, NULL, my_thread, NULL);
    if (rc != 0) {
        fprintf(stderr, "pthread_create failed: %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }

    sleep(3);

    printf("thread: cancel\n");
    rc = pthread_cancel(thread);
    if (rc != 0) {
        fprintf(stderr, "pthread_cancel failed: %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }

    printf("thread: join\n");
    rc = pthread_join(thread, &status);
    if (rc != 0) {
        fprintf(stderr, "pthread_join failed: %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }

    if (status != PTHREAD_CANCELED) {
        fprintf(stderr, "Unexpected thread exit result\n");
        exit(EXIT_FAILURE);
    }

    printf("main exit\n");
    return 0;
}
