#include "lab4.h"

extern int errno;

int main() {
    struct requestmessage request;

    //Get Message Queue for sending
    int msqidsend, msqidrecv, createret = 0, joinret = 0;
    if ((msqidsend = msgget(MQKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("[23]Failed to create Message Queue \n");
        return EXIT_FAILURE;
    }

    //Get Message Queue to receive response from server
    if ((msqidrecv = msgget(MQRKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("[30]Failed to create Message Queue \n");
        return EXIT_FAILURE;
    }

    printf("Client started.\n");
    int amountthreads = 0;
    while (1) {
        char file[64];
        printf("Please enter the absolute file path: \n");
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
            return EXIT_FAILURE;
        }

        /**
         * SEND REQUEST
         */
        printf("Sending request: [%s,%d]\n", request.file, request.amountthreads);
        if (msgsnd(msqidsend, &request, sizeof(struct requestmessage) - sizeof(long), 0) == -1) {
            perror("[69]Message send failed: \n");
            return EXIT_FAILURE;
        }

        printf("vor alloc\n");
        /**
         * GET RESPONSE
        */
        struct serverresponsemessage serverresponse;        //BS
        //struct serverresponsemessage *serverresponse;       //Process finished with exit code 139 (interrupted by signal 11: SIGSEGV)
        //struct serverresponsemessage *serverresponse = malloc(sizeof(struct serverresponsemessage));    //Process finished with exit code 134 (interrupted by signal 6: SIGABRT)

        printf("vor recv\n");

        //Start listening for requests
        int srcv;
        if ((srcv = msgrcv(msqidrecv, &serverresponse, sizeof(struct serverresponsemessage) - sizeof(long), 0, 0)) ==
            -1) {
            perror("[59]Message receive failed \n");
            printf("%d\n", errno);
        } else {

            printf("recv complete -> %d\n", srcv);
            /**
             * DISPLAY RESULTS
             */
            /*printf("Received Results from Server:\n");
            printf("Checksum: %d Execution time: %ldms Bytes read: %ld\n", serverresponse->checksum,
                   serverresponse->executiontime,
                   serverresponse->bytesread);

            int c = 0;
            while (c < 256) {
                printf("%3x|%6d ", c, serverresponse->distribution[c++]);
                printf("%3x|%6d ", c, serverresponse->distribution[c++]);
                printf("%3x|%6d ", c, serverresponse->distribution[c++]);
                printf("%3x|%6d ", c, serverresponse->distribution[c++]);
                printf("%3x|%6d ", c, serverresponse->distribution[c++]);
                printf("%3x|%6d ", c, serverresponse->distribution[c++]);
                printf("%3x|%6d ", c, serverresponse->distribution[c++]);
                printf("%3x|%6d \n", c, serverresponse->distribution[c++]);
            }*/

            printf("Checksum: %d Execution time: %ldms Bytes read: %ld\n", serverresponse.checksum,
                   serverresponse.executiontime,
                   serverresponse.bytesread);

            int c = 0;
            while (c < 256) {
                printf("%3x|%6d ", c, serverresponse.distribution[c++]);
                printf("%3x|%6d ", c, serverresponse.distribution[c++]);
                printf("%3x|%6d ", c, serverresponse.distribution[c++]);
                printf("%3x|%6d ", c, serverresponse.distribution[c++]);
                printf("%3x|%6d ", c, serverresponse.distribution[c++]);
                printf("%3x|%6d ", c, serverresponse.distribution[c++]);
                printf("%3x|%6d ", c, serverresponse.distribution[c++]);
                printf("%3x|%6d \n", c, serverresponse.distribution[c++]);
            }

            /**
             * FREE UP MEMORY
             */
        }
    }
}