#include "lab4.h"

void *responseThread(void *args) {

    int msqidrecv = *((int *) args);
    ssize_t sizereceived = 0;

    struct serverresponsemessage *response = malloc(sizeof(struct serverresponsemessage));
    while (1) {
        if ((sizereceived = msgrcv(msqidrecv, &response, sizeof(struct serverresponsemessage) - sizeof(long), 0, 0)) == -1) {
            perror("Message receive failed:\n");
            //killAllMessageQueues(msqidrecv, 0);
            return (void *) EXIT_FAILURE;
        } else {
            printf("received response.\n");
            //printf("bytes read: %d checksum: %d\n", response->bytesread, response->checksum);
        }

        //pthread_exit(response);
        return EXIT_SUCCESS;
    }
}
