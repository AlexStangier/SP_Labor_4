#include "lab4.h"

void killAllMessageQueues(int clientqueue, int serverqueue) {
    char command[256];
    sprintf(command, "ipcrm -q %d -q %d", clientqueue, serverqueue);
    system(command);
}

int main() {

    //Get Message Queue for sending
    int msqidsend, msqidrecv;
    if ((msqidsend = msgget(MQKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("Failed to create Message Queue \n");
        killAllMessageQueues(msqidsend, 0);
        return EXIT_FAILURE;
    }

    //Get Message Queue to receive response from server
    if ((msqidrecv = msgget(1338, IPC_CREAT | S_IRWXU)) == -1) {
        perror("Failed to create Message Queue \n");
        killAllMessageQueues(msqidsend, msqidrecv);
        return EXIT_FAILURE;
    }

    printf("send queue: %d\n", msqidsend);
    printf("recv queue: %d\n", msqidrecv);

    /**
     * GET RESPONSE
    */

    pthread_t tid = 0;
    pthread_create(&tid, NULL, responseThread, (void *) &msqidrecv);


    int amountthreads = 0;
    while (1) {
        struct requestmessage request;
        char file[64];
        printf("Please enter the file name: \n");
        scanf("%64s", file);

        printf("Please enter the amount of Threads: \n");
        scanf("%d", &amountthreads);

        memcpy(request.file, file, 64);
        request.amountthreads = amountthreads;
        request.responsequeuekey = msqidrecv;

        /**
         * SEND REQUEST
         */

        printf("sending request: [%s,%d]\n", request.file, request.amountthreads);
        if (msgsnd(msqidsend, &request, sizeof(struct requestmessage) - sizeof(long), 0) == -1) {
            perror("Message send failed: \n");
            killAllMessageQueues(msqidsend, 0);
            return EXIT_FAILURE;
        }

        struct serverresponsemessage *response;
        pthread_join(tid, (void **) &response);

        printf("received from server: cs:%d br:%d\n", response->checksum, response->bytesread);
    }

}
