#include "clientQueue.h"
static struct clientPaths clientQueue[CLIENT_QUEUE_SZ];
static pthread_mutex_t clientQueueLock =PTHREAD_MUTEX_INITIALIZER;

static int queuedNum = 0;
static pthread_mutex_t queuedNumLock=PTHREAD_MUTEX_INITIALIZER;

/*
    Having the producer and consumer share a condition would force us to rely
    on brodcast to avoid dealocks.
    Ie. the producer is waiting for the consumers, but the consumers
    are only signaling each other due to some quirk of pthread_signal
    scheduling
*/
static pthread_cond_t enqueuedCond=PTHREAD_COND_INITIALIZER;
static pthread_cond_t dequeuedCond=PTHREAD_COND_INITIALIZER;


//Caller will wait if queue is full
//returns 0 on success
int enqueueClient(char * responsePipePath, char * requestPipePath){
    char * newRespPath= (char *) calloc(41, sizeof(char));
    char * newReqPath= (char *) calloc(41, sizeof(char));
    strcpy(newRespPath,responsePipePath );
    strcpy(newReqPath,requestPipePath);
    
    if(pthread_mutex_lock(&queuedNumLock))   return 1;
    while(queuedNum==CLIENT_QUEUE_SZ){
        //printf("P: queuedNum: %d; clientQueue: %s:%s\n", queuedNum,clientQueue[0].responsePipePath,clientQueue[0].requestPipePath);
        pthread_cond_wait(&dequeuedCond, &queuedNumLock);
    }

    if(pthread_mutex_lock(&clientQueueLock))   return 1;

    clientQueue[queuedNum].responsePipePath=newRespPath;
    clientQueue[queuedNum].requestPipePath=newReqPath;
    queuedNum++;

    pthread_mutex_unlock(&clientQueueLock);
    pthread_cond_signal(&enqueuedCond);
    pthread_mutex_unlock(&queuedNumLock);
    

    return 0;
}


//Caller will wait if queue is empty
//returns 0 on success
int consumeClient(char ** responsePipeFd, char ** requestPipeFd){

    if(pthread_mutex_lock(&queuedNumLock))   return 1;
    while(queuedNum==0){
        //printf("C: queuedNum: %d; clientQueue: %s:%s\n", queuedNum,clientQueue[0].responsePipePath,clientQueue[0].requestPipePath);
        pthread_cond_wait(&enqueuedCond, &queuedNumLock);
    }

    if(pthread_mutex_lock(&clientQueueLock))   return 1;

    *responsePipeFd=clientQueue[0].responsePipePath;
    *requestPipeFd=clientQueue[0].requestPipePath;
    
    if(CLIENT_QUEUE_SZ > 1){
    //it's just char pointers, this is fine, dw
    memmove(clientQueue, &(clientQueue[1]), (CLIENT_QUEUE_SZ-1)*sizeof(struct clientPaths));
    }
    clientQueue[queuedNum-1].responsePipePath=NULL;
    clientQueue[queuedNum-1].requestPipePath=NULL;

    queuedNum--;

    pthread_mutex_unlock(&clientQueueLock);
    pthread_cond_broadcast(&dequeuedCond);
    pthread_mutex_unlock(&queuedNumLock);
    
    return 0;
}
