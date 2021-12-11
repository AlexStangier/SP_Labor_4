#include "lab4.h"

void *responseThread(void *args) {

    int msqidrecv = *((int *) args);
    ssize_t sizereceived = 0;

    struct serverresponsemessage *response = malloc(sizeof(struct serverresponsemessage));
    response->checksum = 420;
    response->bytesread = 69;

    while (1) {
        /*sizereceived = msgrcv(msqidrecv, &response, sizeof(struct serverresponsemessage) - sizeof(long), 0, 0);
        if (sizereceived == -1) {
            perror("Message receive failed:\n");
            killAllMessageQueues();
            return (void *) EXIT_FAILURE;
        }*/

        //printf("received response.\n");
        //printf("bytes read: %d checksum: %d\n", response->bytesread, response->checksum);
        usleep(10000000);
        pthread_exit((void *) response);
    }

    pthread_exit((void *) response);

    return EXIT_SUCCESS;
}
