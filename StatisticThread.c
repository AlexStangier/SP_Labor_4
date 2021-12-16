#include "lab4.h"

void *statisticThread(void *args) {
    /*
    struct threadworkermessage *local = (struct threadworkermessage *)malloc(sizeof(struct threadworkermessage));
    if (local == NULL) {
        perror("[8]Malloc failed\n");
        return (void *) EXIT_FAILURE;
    }
    memcpy(local, args, sizeof(struct threadworkermessage));
     */

    struct threadworkermessage *local = (struct threadworkermessage *) args;

    char *buffer = malloc(sizeof(char) * local->blocksize);
    if (buffer == NULL) {
        perror("[15]Malloc failed\n");
        return (void *) EXIT_FAILURE;
    }

    printf("th:br: %lld\n", local->blocksize);

    struct threadresponsemessage *response = (struct threadresponsemessage *) malloc(
            sizeof(struct threadresponsemessage));
    if (response == NULL) {
        perror("[21]Malloc failed\n");
        return (void *) EXIT_FAILURE;
    }

    pthread_mutex_lock(&lock);

    /**
     * READ DATA
     */
    int rc = 0;
    if ((rc = fseek(local->fd, local->lowerbound, SEEK_SET) != 0)) {
        perror("[30]Failed to set read pointer \n");
    }

    long long bytesread = 0;
    bytesread = fread(buffer, sizeof(char), local->blocksize, local->fd);

    for (int j = 0; j < CHARSETLENGTH; j++) {
        response->distribution[j] = 0;
    }

    pthread_mutex_unlock(&lock);
    /**
     * ANALYSE DATA
     **/
    /*
   response->bytesread = 0;
   response->checksum = 0;
   char *buffer2;
   char test[1];
   for (long long i = 0; i < local->blocksize; i++) {
       fread(&buffer2, 1, 1, local->fd);
       r//esponse->checksum += test[0];
       response->bytesread++;
       for (long long j = 0; j < CHARSETLENGTH; j++) {
           if ((char *) j == buffer2)response->distribution[j] += 1;
       }
   }
     */

    for (long long i = 0; i < bytesread; i++) {
        response->checksum += buffer[i];
        for (int j = 0; j < CHARSETLENGTH; j++) {
            if ((char) j == (char) buffer[i])response->distribution[j] += 1;
        }
        //printf("%c\n", buffer[i]);
        //response->distribution[buffer[i]] += 1;
    }


    response->bytesread = bytesread;

    /*int c = 0;
    while (c < 256) {
        printf("%3x|%6d ", c, response->distribution[c++]);
        printf("%3x|%6d ", c, response->distribution[c++]);
        printf("%3x|%6d ", c, response->distribution[c++]);
        printf("%3x|%6d ", c, response->distribution[c++]);
        printf("%3x|%6d ", c, response->distribution[c++]);
        printf("%3x|%6d ", c, response->distribution[c++]);
        printf("%3x|%6d ", c, response->distribution[c++]);
        printf("%3x|%6d \n", c, response->distribution[c++]);
    }
*/



    /**
     * CLEANUP AND EXIT
     */
    //free(args);
    pthread_exit(response);
}
