#include "lab4.h"

void *responseThread(void *args) {

    int msqidrecv = *((int *) args);
    ssize_t sizereceived;

    struct serverresponsemessage *response = malloc(sizeof(struct serverresponsemessage));

    sizereceived = msgrcv(msqidrecv, &response, sizeof(struct serverresponsemessage) - sizeof(long), 0, 0);
    if (sizereceived == -1) {
        perror("Message receive failed:\n");
        pthread_exit((void *) NULL);
    }

    /*struct serverresponsemessage *local = malloc(sizeof(struct serverresponsemessage));
    local->bytesread = response.bytesread;
    local->checksum = response.checksum;
    local->executiontime = response.executiontime;
    memcpy(local->distribution, response.distribution, sizeof(response.distribution));*/

    pthread_exit(response);
}
