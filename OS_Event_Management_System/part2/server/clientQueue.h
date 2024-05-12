#include "common/constants.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define CLIENT_QUEUE_SZ MAX_SESSION_COUNT*10
struct clientPaths{
    char * responsePipePath;
    char * requestPipePath;
};


int enqueueClient(char * responsePipePath, char * requestPipePath);
int consumeClient(char ** responsePipeFd, char ** requestPipeFd);