#include "lab4.h"

int killAllMessageQueues() {
    char *command = malloc(256 * sizeof(char));
    int written = 0;
    if ((written = sprintf(command, "ipcrm -q %d -q %d", MQKEY, MQRKEY) > 0)) {
        system(command);
        free(command);
        return EXIT_SUCCESS;
    } else {
        perror("failed to destroy message queues.\n");
        return EXIT_FAILURE;
    }
}

int main() {
    struct requestmessage request;
    struct serverresponsemessage *response;

    //Get Message Queue for sending
    int msqidsend, msqidrecv, createret = 0, joinret = 0;
    if ((msqidsend = msgget(MQKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("Failed to create Message Queue \n");
        killAllMessageQueues();
        return EXIT_FAILURE;
    }

    //Get Message Queue to receive response from server
    if ((msqidrecv = msgget(MQRKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("Failed to create Message Queue \n");
        killAllMessageQueues();
        return EXIT_FAILURE;
    }

    printf("send queue: %d\n", msqidsend);
    printf("recv queue: %d\n", msqidrecv);

    /**
     * GET RESPONSE
    */

    pthread_t tid = 0;
    if ((createret = pthread_create(&tid, NULL, responseThread, (void *) &msqidrecv)) == 0) {
        printf("created listening thread\n");
    } else {
        perror("failed to create listening thread:\n");
    }


    int amountthreads = 0;
    while (1) {

        char file[64];
        printf("Please enter the file name: \n");
        scanf("%64s", file);

        printf("Please enter the amount of Threads: \n");
        scanf("%d", &amountthreads);

        memcpy(request.file, file, 64);
        request.amountthreads = amountthreads;
        request.responsequeuekey = MQRKEY;

        /**
         * SEND REQUEST
         */

        printf("sending request: [%s,%d]\n", request.file, request.amountthreads);
        if (msgsnd(msqidsend, &request, sizeof(struct requestmessage) - sizeof(long), 0) == -1) {
            perror("Message send failed: \n");
            killAllMessageQueues();
            return EXIT_FAILURE;
        } else {
            printf("request sent.\n");
        }

        printf("waiting for response.\n");
        if ((joinret = pthread_join(tid, (void **) &response)) == 0) {
            printf("currently waiting for response.\n");
        } else {
            perror("failed to receive resopnse\n");
        }

        //printf("received from server: cs:%d br:%d\n", response->checksum, response->bytesread);
        killAllMessageQueues();
        printf("message queues destroyed.\n");
        return EXIT_SUCCESS;
    }
}
