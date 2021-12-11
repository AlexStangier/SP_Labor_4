#include "lab4.h"

extern int errno;

void *statisticThread(void *args) {
    int seek = 0;
//    ssize_t bytesread = 0;

    struct threadworkermessage *local = malloc(sizeof(struct threadworkermessage));
    memcpy(local, args, sizeof(struct threadworkermessage));

    char *buffer = malloc(sizeof(char) * local->blocksize);

    struct threadresponsemessage *response = malloc(sizeof(struct threadresponsemessage));
    response->bytesread = 1;
    response->checksum = 1;
    response->executiontime = 1;

    pthread_mutex_lock(&lock);

    //printf("Thread: lower:%d upper:%d blocksize:%d\n", local->lowerbound, local->upperbound, local->blocksize);

    /*if ((seek = lseek(local->fd, local->lowerbound, SEEK_CUR)) < 0) {
        perror("Error while resetting the Read Pointer\n");
        pthread_mutex_unlock(&lock);
        free(buffer);
        free(local);
        return (void *) NULL;
    }*/


    //printf("begin read\n");
    /*bytesread = read(local->fd, buffer, local->blocksize);
    if (bytesread > 0) {
        printf("read: %zd\n", bytesread);
    } else {
        printf("read: %d\n", errno);
        perror("read failed: ");
        pthread_mutex_unlock(&lock);
        return (void *) NULL;
    }*/

    pthread_mutex_unlock(&lock);

    free(buffer);
    free(local);
    pthread_exit((void *) response);
    return EXIT_SUCCESS;
}
