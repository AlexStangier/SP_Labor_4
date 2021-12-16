#include "lab4.h"

void *statisticThread(void *args) {
    struct threadworkermessage *local = (struct threadworkermessage *) args;

    char *buffer = malloc(sizeof(char) * local->blocksize);
    if (buffer == NULL) {
        perror("[15]Malloc failed\n");
        return (void *) EXIT_FAILURE;
    }

    struct threadresponsemessage *response = (struct threadresponsemessage *) malloc(
            sizeof(struct threadresponsemessage));
    if (response == NULL) {
        perror("[21]Malloc failed\n");
        return (void *) EXIT_FAILURE;
    }

    /**
     * READ DATA
     */
    pthread_mutex_lock(&lock);
    int rc = 0;
    if ((rc = fseek(local->fd, 0L, SEEK_SET) != 0)) {
        perror("[30]Failed to set read pointer \n");
    }

    rc = 0;
    if ((rc = fseek(local->fd, local->lowerbound, SEEK_SET) != 0)) {
        perror("[35]Failed to set read pointer \n");
    }

    long long bytesread = 0;
    bytesread = fread(buffer, sizeof(char), local->blocksize, local->fd);

    for (int j = 0; j < CHARSETLENGTH; j++) {
        response->distribution[j] = 0;
    }

    pthread_mutex_unlock(&lock);

    /**
     * ANALYSE DATA
     **/
    for (long long i = 0; i < bytesread; i++) {
        response->checksum += buffer[i];
        for (int j = 0; j < CHARSETLENGTH; j++) {
            if ((char) j == (char) buffer[i])response->distribution[j] += 1;
        }
    }

    response->bytesread = bytesread;

    /**
     * CLEANUP AND EXIT
     */
    free(buffer);
    pthread_exit(response);
}
