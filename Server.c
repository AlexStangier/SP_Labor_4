#include "lab4.h"

pthread_mutex_t lock;

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
    //get message queue
    pthread_mutex_t lock;
    int msqid, msqidres;
    short fd;
    if ((msqid = msgget(MQKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("Failed to create Message Queue:");
        killAllMessageQueues();
        return EXIT_FAILURE;
    }

    printf("Server started.\n");

    while (1) {
        struct requestmessage request;
        int sizereceived;

        //Start listening fro requests
        if ((sizereceived = msgrcv(msqid, &request, sizeof(struct requestmessage) - sizeof(long), 0, 0)) == -1) {
            perror("[19]Message receive failed ");
            killAllMessageQueues();
            return EXIT_FAILURE;
        } else {
            //check termination
            if (request.file[0] == 'q' && request.file[1] != '0') {
                killAllMessageQueues();
                printf("Server Shutdown.\n");
            }

            if (pthread_mutex_init(&lock, NULL) != 0) {
                perror("[48]Mutex failed\n");
            }

            if ((fd = open(request.file, O_RDONLY) == -1)) {
                perror("[52]Failed to open the specified file: ");
            }

            //Get filesize
            struct stat sb;
            if (stat(request.file, &sb) == -1) {
                perror("stat failed");
            }
            int filesize = sb.st_size;

            printf("Received request from Client: [%s, %d] -> file is %dB big\n", request.file, request.amountthreads,
                   filesize);

            /**
             * START THREADS
             */

            pthread_t tids[request.amountthreads];
            for (int i = 0; i < request.amountthreads; i++) {
                struct threadworkermessage startmsg;
                pthread_t tid;

                //calc thread boundaries for reading the data
                if (i == 0) { startmsg.lowerbound = 0; }
                else {
                    startmsg.lowerbound = (int) (i * (filesize / request.amountthreads));
                }

                startmsg.upperbound =
                        (int) ((i + 1) * (filesize / request.amountthreads));

                if (i == request.amountthreads - 1) {
                    startmsg.upperbound += (filesize % request.amountthreads);
                }

                printf("Creating Statistic Thread with bounds [%d;%d]\n", startmsg.lowerbound, startmsg.upperbound);

                startmsg.fd = fd;
                startmsg.blocksize = startmsg.upperbound - startmsg.lowerbound;

                //Start thread
                pthread_create(&tid, NULL, statisticThread, &startmsg);
                tids[i] = tid;
            }
            /**
             * DATA EVALUATION
             */
            struct serverresponsemessage serverresponse;
            if ((msqidres = msgget(request.responsequeuekey, IPC_CREAT | S_IRWXU)) == -1) {
                perror("[83]Failed to create Message Queue to send result:");
                killAllMessageQueues();
                return EXIT_FAILURE;
            }

            //reset entries
            serverresponse.bytesread = 0;
            serverresponse.checksum = 0;


            int i = 0;
            for (i = 0; i < request.amountthreads; i++) {
                struct threadresponsemessage *threadata;
                pthread_join(tids[i], (void **) &threadata);

                serverresponse.bytesread += threadata->bytesread;
                serverresponse.checksum += threadata->checksum;
            }

            printf("Results: br: %d cs: %d\n", serverresponse.bytesread, serverresponse.checksum);

            /**
             * SEND RESULT
            */
            if (msgsnd(msqidres, &serverresponse, sizeof(struct serverresponsemessage) - sizeof(long), 0) == -1) {
                perror("[120]Message send failed: \n");
                killAllMessageQueues();
                return EXIT_FAILURE;
            }

            printf("Response send.\n");
        }
    }
}
