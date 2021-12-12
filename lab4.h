#ifndef lab4
#define lab4

#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <math.h>
#include <unistd.h>

#define MQKEY 1337
#define MQRKEY 1338

extern pthread_mutex_t lock;

struct requestmessage {
    char file[64];
    int amountthreads;
    int responsequeuekey;
};

struct threadworkermessage {
    FILE *fd;
    int lowerbound;
    int upperbound;
    int blocksize;
};

struct serverresponsemessage {
    int checksum;
    int bytesread;
    int distribution[256];
};

struct threadresponsemessage {
    int checksum;
    size_t bytesread;
    int distribution[256];
    int executiontime;
};

void *statisticThread(void *args);

void *responseThread(void *args);

int killAllMessageQueues();

#endif

//gcc -lm -lpthread Server.c StatisticThread.c -o server -Wall
//gcc -lm -lpthread Client.c -o client -Wall
//ipcs -q
//ipcrm -Q 0x539 -Q 0x53a
