#include "lab4.h"

pthread_mutex_t lock;

/**
 * Splits the supplied file into n threads
 * @param n amount of partitions
 * @param filesize size of the file
 * @param fd Filepointer for the file
 * @return array of completed threadworkermessages
 */
void *startThreads(int n, long filesize, FILE *fd) {
    pthread_t *threadIds = malloc(n * sizeof(pthread_t));
    for (int i = 0; i < n; i++) {
        struct threadworkermessage *startmsg = malloc(sizeof(struct threadworkermessage));

        //calc thread boundaries for reading the data
        if (i == 0) { startmsg->lowerbound = 0; }
        else {
            startmsg->lowerbound = (int) (i * (filesize / n));
        }

        startmsg->upperbound = (int) ((i + 1) * (filesize / n));

        int remainder = filesize % n;
        if (i == (n - 1)) startmsg->upperbound += remainder;

        startmsg->fd = fd;
        startmsg->blocksize = startmsg->upperbound - startmsg->lowerbound;

        pthread_t tid;
        pthread_create(&tid, NULL, statisticThread, startmsg);

        printf("Created Statistic Thread with bounds [%ld;%ld] -> Blocksize => [%ld]\n", startmsg->lowerbound,
               startmsg->upperbound, startmsg->blocksize);

        threadIds[i] = tid;
    }
    return threadIds;
}

int main() {
    //get message queue
    system("ipcrm -Q 0x539 -Q 0x53a");
    if (pthread_mutex_init(&lock, NULL) != 0) {
        perror("[48]Mutex failed\n");
        return EXIT_FAILURE;
    }
    struct timeval startTime, endTime;
    int msqid, msqidres;
    if ((msqid = msgget(MQKEY, IPC_CREAT | S_IRWXU)) == -1) {
        perror("Failed to create Message Queue:\n");
        return EXIT_FAILURE;
    }

    printf("Server started.\n");

    while (1) {
        struct requestmessage request;

        //Start listening fro requests
        int sizereceived;
        if ((sizereceived = msgrcv(msqid, &request, sizeof(struct requestmessage) - sizeof(long), 0, 0)) == -1) {
            perror("[19]Message receive failed \n");
            return EXIT_FAILURE;
        } else {
            gettimeofday(&startTime, NULL);

            ///Users/alexstangier/Desktop/Labor4/abc
            ///Users/alexstangier/Desktop/Labor4/lab
            ///Users/alexstangier/Desktop/Labor4/lab.large
            FILE *fd = fopen("/Users/alexstangier/Desktop/Labor4/lab", "r");
            if (fd == NULL) {
                printf("File Not Found!\n");
                return EXIT_FAILURE;
            }

            int rc = 0;
            if ((rc = fseek(fd, 0L, SEEK_END) != 0)) {
                perror("[54]Failed to set read pointer\n");
                return EXIT_FAILURE;
            }

            long int filesize = ftell(fd);
            if ((rc = fseek(fd, 0L, SEEK_SET) != 0)) {
                perror("[59]Failed to set read pointer\n");
                return EXIT_FAILURE;
            }

            printf("Received request from Client: [%s, %d] -> file is %ldB big\n", request.file, request.amountthreads,
                   filesize);

            /**
             * START THREADS
             */
            pthread_t *completedThreads = startThreads(request.amountthreads, filesize, fd);

            /**
             * DATA EVALUATION
             */
            struct serverresponsemessage *serverresponse = malloc(sizeof(struct serverresponsemessage));
            struct threadresponsemessage *threadData[request.amountthreads];

            int threadsLeft = request.amountthreads;
            for (int i = 0; i < request.amountthreads; i++) {
                pthread_join(completedThreads[i], (void **) &threadData[i]);
                printf("server => cs:%d br:%zu\n", threadData[i]->checksum, threadData[i]->bytesread);
                threadsLeft--;
            }

            gettimeofday(&endTime, NULL);

            if (threadsLeft == 0) {
                for (int i = 0; i < request.amountthreads; i++) {
                    serverresponse->bytesread += threadData[i]->bytesread;
                    serverresponse->checksum += threadData[i]->checksum;
                    serverresponse->executiontime =
                            ((endTime.tv_sec - startTime.tv_sec) * 1000000) + ((endTime.tv_usec - startTime.tv_usec));
                    for (int j = 0; j < CHARSETLENGTH; j++) {
                        serverresponse->distribution[j] += threadData[i]->distribution[j];
                    }
                }
            }

            printf("final br: %zu cs: %d\n", serverresponse->bytesread, serverresponse->checksum);
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
            }

            /**
             * SEND RESULT AND FREE UP RESOURCES
            */
            if ((msqidres = msgget(request.responsequeuekey, IPC_CREAT | S_IRWXU)) == -1) {
                perror("[83]Failed to create Message Queue to send result:\n");
                return EXIT_FAILURE;
            }
            if (msgsnd(msqidres, &serverresponse, sizeof(struct serverresponsemessage) - sizeof(long), 0) == -1) {
                perror("[113]Message send failed: \n");
                return EXIT_FAILURE;
            }

            printf("Response send. -> %lub\n", sizeof(struct serverresponsemessage));
            fclose(fd);
        }
    }
}
