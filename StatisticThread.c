#include "lab4.h"

extern int errno;

void *statisticThread(void *args) {
    size_t bytesread = 0;
    int rc = 0;

    struct threadworkermessage *local = malloc(sizeof(struct threadworkermessage));
    memcpy(local, args, sizeof(struct threadworkermessage));

    char *buffer = malloc(sizeof(char) * local->blocksize);

    struct threadresponsemessage *response = malloc(sizeof(struct threadresponsemessage));

    /**
     * READ DATA
     */

    pthread_mutex_lock(&lock);

    if ((rc = fseek(local->fd, 0L, SEEK_SET) != 0)) {
        perror("[23]Failed to set read pointer \n");
    }

    bytesread = fread(buffer, sizeof(char), local->blocksize, local->fd);

    pthread_mutex_unlock(&lock);

    /**
     * ANALYSE DATA
     **/

    //get char distribution
    for (size_t i = 0; i < bytesread; i++) {
        response->checksum += buffer[i];
        response->distribution[buffer[i]] += 1;
    }

    response->bytesread = bytesread;

    /**
     * CLEANUP AND EXIT
     */

    free(buffer);
    free(local);
    pthread_exit((void *) response);
}
