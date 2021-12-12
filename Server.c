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
    if ((msqid = msgget(MQKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("Failed to create Message Queue:\n");
        killAllMessageQueues();
        return EXIT_FAILURE;
    }

    printf("Server started.\n");

    while (1) {
        struct requestmessage request;

        //Start listening fro requests
        int sizereceived;
        if ((sizereceived = msgrcv(msqid, &request, sizeof(struct requestmessage) - sizeof(long), 0, 0)) == -1) {
            perror("[19]Message receive failed \n");
            killAllMessageQueues();
            return EXIT_FAILURE;
        } else {
            if (pthread_mutex_init(&lock, NULL) != 0) {
                perror("[48]Mutex failed\n");
            }

            FILE *fd = fopen("/Users/alexstangier/Desktop/Labor 4/lab", "r");
            if (fd == NULL) {
                printf("File Not Found!\n");
                return EXIT_FAILURE;
            }

            int rc = 0;
            if ((rc = fseek(fd, 0L, SEEK_END) != 0)) {
                perror("[54]Failed to set read pointer\n");
            }

            long int filesize = ftell(fd);
            if ((rc = fseek(fd, 0L, SEEK_SET) != 0)) {
                perror("[59]Failed to set read pointer\n");
            }

            printf("Received request from Client: [%s, %d] -> file is %ldB big\n", request.file, request.amountthreads,
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

            if ((msqidres = msgget(request.responsequeuekey, IPC_CREAT | S_IRWXU)) == -1) {
                perror("[83]Failed to create Message Queue to send result:\n");
                killAllMessageQueues();
                return EXIT_FAILURE;
            }

            int i = 0;
            struct serverresponsemessage serverresponse;
            for (i = 0; i < request.amountthreads; i++) {
                struct threadresponsemessage *threadata;
                pthread_join(tids[i], (void **) &threadata);

                serverresponse.bytesread += threadata->bytesread;
                serverresponse.checksum += threadata->checksum;
            }

            /**
             * SEND RESULT AND FREE UP RESSOURCES
            */
            if (msgsnd(msqidres, &serverresponse, sizeof(struct serverresponsemessage) - sizeof(long), 0) == -1) {
                perror("[120]Message send failed: \n");
                killAllMessageQueues();
                return EXIT_FAILURE;
            }

            printf("Response send.\n");
            fclose(fd);
        }
    }
}
