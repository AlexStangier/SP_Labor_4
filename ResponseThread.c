#include "lab4.h"

void *responseThread(void *args) {

    int msqidrecv = *((int *) args);
    ssize_t sizereceived;

    struct serverresponsemessage response;

    sizereceived = msgrcv(msqidrecv, &response, sizeof(struct serverresponsemessage) - sizeof(long), 0, 0);
    if (sizereceived == -1) {
        perror("Message receive failed:\n");
        killAllMessageQueues();
        pthread_exit((void *) NULL);
    }

    struct serverresponsemessage *p = malloc(sizeof(struct serverresponsemessage));
    p->bytesread = response.bytesread;
    p->checksum = response.checksum;
    memcpy(p->distribution, response.distribution, sizeof(response.distribution));


    printf("bytes read: %d checksum: %d\n", response.bytesread, response.checksum);
    pthread_exit((void *) p);
}
