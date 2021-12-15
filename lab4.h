#ifndef lab4
#define lab4
#define MQKEY 1337
#define MQRKEY 1338
#define CHARSETLENGTH 256

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
#include <sys/time.h>

extern pthread_mutex_t lock;

struct requestmessage {
    char file[64];
    int amountthreads;
    int responsequeuekey;
};

struct threadworkermessage {
    void *fd;
    long lowerbound;
    long upperbound;
    long blocksize;
};

struct serverresponsemessage {
    char checksum;
    size_t bytesread;
    int distribution[CHARSETLENGTH];
    long executiontime;
};

struct threadresponsemessage {
    char checksum;
    size_t bytesread;
    int distribution[CHARSETLENGTH];
};

void *statisticThread(void *args);

void *responseThread(void *args);

#endif

//gcc -lm -lpthread Server.c StatisticThread.c -o server -Wall
//gcc -lm -lpthread Client.c -o client -Wall
//ipcs -q
//ipcrm -Q 0x539 -Q 0x53a
