#include "lab4.h"

void *statisticThread(void *args) {
    pthread_mutex_lock(&lock);

    struct threadworkermessage *local = malloc(sizeof(struct threadworkermessage));
    if (local == NULL) {
        perror("[11]Malloc failed\n");
        return (void *) EXIT_FAILURE;
    }
    memcpy(local, args, sizeof(struct threadworkermessage));

    char *buffer = malloc(sizeof(char) * local->blocksize);
    if (buffer == NULL) {
        perror("[18]Malloc failed\n");
        return (void *) EXIT_FAILURE;
    }

    struct threadresponsemessage *response = malloc(sizeof(struct threadresponsemessage));
    if (response == NULL) {
        perror("[31]Malloc failed\n");
        return (void *) EXIT_FAILURE;
    }

    /**
     * READ DATA
     */
    int rc = 0;
    if ((rc = fseek(local->fd, local->lowerbound, SEEK_SET) != 0)) {
        perror("[23]Failed to set read pointer \n");
    }

    size_t bytesread = 0;
    bytesread = fread(buffer, sizeof(unsigned char), local->blocksize , local->fd);

    /**
     * ANALYSE DATA
     **/
    for (size_t i = 0; i < bytesread; i++) {
        response->checksum += buffer[i];
        response->distribution[buffer[i]] += 1;
    }
    response->bytesread = bytesread;

    pthread_mutex_unlock(&lock);

    /**
     * CLEANUP AND EXIT
     */
    free(args);
    pthread_exit((void *) response);
}
