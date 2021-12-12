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

    //Get Message Queue for sending
    int msqidsend, msqidrecv, createret = 0, joinret = 0;
    if ((msqidsend = msgget(MQKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("[23]Failed to create Message Queue \n");
        killAllMessageQueues();
        return EXIT_FAILURE;
    }

    //Get Message Queue to receive response from server
    if ((msqidrecv = msgget(MQRKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("[30]Failed to create Message Queue \n");
        killAllMessageQueues();
        return EXIT_FAILURE;
    }

    printf("Client started.\n");
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
         * START LISTENING
         */

        pthread_t tid = 0;
        if ((createret = pthread_create(&tid, NULL, responseThread, (void *) &msqidrecv)) != 0) {
            perror("[44]Failed to create listening thread:\n");
            killAllMessageQueues();
            return EXIT_FAILURE;
        }

        /**
         * SEND REQUEST
         */

        printf("Sending request: [%s,%d]\n", request.file, request.amountthreads);
        if (msgsnd(msqidsend, &request, sizeof(struct requestmessage) - sizeof(long), 0) == -1) {
            perror("[69]Message send failed: \n");
            killAllMessageQueues();
            return EXIT_FAILURE;
        }

        /**
         * GET RESPONSE
        */

        printf("Waiting for response.\n");
        struct serverresponsemessage *response;
        if ((joinret = pthread_join(tid, (void **) &response)) == 0) {
            printf("Received response.\n");
        } else {
            perror("[79]Failed to receive response\n");
            return EXIT_FAILURE;
        }

        /**
         * DISPLAY RESULTS
         */
        printf("Received From thread: %d\n", response->checksum);

        /**
         * FREE UP MEMORY
         */
         free(response);
    }
}
