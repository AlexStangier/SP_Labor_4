//
// Created by Alexander Stangier on 15.12.21.
//
#include "lab4.h"

void main() {
    int msqidsend, msqidrecv;

    if ((msqidsend = msgget(420, IPC_CREAT | S_IRWXU)) == -1) {
        perror("[23]Failed to create Message Queue \n");
        return EXIT_FAILURE;

    }
    if ((msqidrecv = msgget(69, IPC_CREAT | S_IRWXU)) == -1) {
        perror("[23]Failed to create Message Queue \n");
        return EXIT_FAILURE;
    }

    while (1) {

        //recv
        if ((msqidrecv = msgget(MQRKEY, IPC_CREAT | S_IRWXU)) == -1) {
            perror("[30]Failed to create Message Queue \n");
            return EXIT_FAILURE;
        }

        //send
        if (msgsnd(msqidsend, &request, sizeof(struct requestmessage) - sizeof(long), 0) == -1) {
            perror("[69]Message send failed: \n");
            return EXIT_FAILURE;
        }

    }
}