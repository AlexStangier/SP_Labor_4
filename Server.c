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
        if ((sizereceived = msgrcv(msqid, &request, sizeof(struct requestmessage) - sizeof(long), 0, 0)) == -1) {
            perror("[19]Message receive failed ");
            killAllMessageQueues();
            return EXIT_FAILURE;
        } else {
            //check termination
            if (request.file[0] == 'q' && request.file[1] != '0') {
                system("ipcrm -a");
                printf("Server Shutdown.\n");
            }

            if (pthread_mutex_init(&lock, NULL) != 0) {
                perror("mutex failed\n");
            }

            if ((fd = open(request.file, O_RDONLY) == -1)) {
                perror("Failed to open the specified file: ");
            }

            printf("fd server: %d\n", fd);

            printf("received from client: %s, %d\n", request.file, request.amountthreads);

            struct stat sb;
            if (stat(request.file, &sb) == -1) {
                perror("stat failed");
            }

            int filesize = sb.st_size;

            printf("filesize: %d\n", filesize);

            for (int i = 0; i < request.amountthreads; i++) {
                struct threadworkermessage startmsg;
                pthread_t tid;

                //calc lower bound of data
                if (i == 0) { startmsg.lowerbound = 0; }
                else {
                    startmsg.lowerbound = (int) (i * (filesize / request.amountthreads));
                }

                startmsg.upperbound =
                        (int) ((i + 1) * (filesize / request.amountthreads));

                if (i == request.amountthreads - 1) {
                    startmsg.upperbound += (filesize % request.amountthreads);
                }

                printf("creating stat thread with read from: %d to %d\n", startmsg.lowerbound, startmsg.upperbound);

                startmsg.fd = fd;

                startmsg.blocksize = startmsg.upperbound - startmsg.lowerbound;

                pthread_create(&tid, NULL, statisticThread, &startmsg);
                struct threadresponsemessage *response;
                pthread_join(tid, (void **) &response);

                printf("thread exited bytes_read:%d cs:%d ex time:%d\n", response->bytesread, response->checksum,
                       response->executiontime);

                printf("before response send\n");

                //do statistics stuff
                if ((msqidres = msgget(request.responsequeuekey, IPC_CREAT | S_IRWXU)) == -1) {
                    perror("[83]Failed to create Message Queue to send result:");
                    killAllMessageQueues();
                    return EXIT_FAILURE;
                }

                struct serverresponsemessage serverresponse;
                serverresponse.checksum = response->checksum;
                serverresponse.bytesread = response->bytesread;

                if (msgsnd(msqidres, &serverresponse, sizeof(struct serverresponsemessage) - sizeof(long), 0) == -1) {
                    perror("Message send failed: \n");
                    killAllMessageQueues();
                } else {
                    printf("response send\n");
                }
            }
        }
    }

    pthread_mutex_destroy(&lock);

    return EXIT_SUCCESS;
}
