#pragma once
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "operations.h"
#include "parser.h"

//includes and declarations for main not in the provided code base
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define JOB_FILE_EXTENSION ".jobs"
#define JOB_OUTPUT_EXTENSION ".out"
#define FILE_IGNORED_STATUS       2

#define JOB_DONE_VALUE            0
#define JOB_FILE_DONE_VALUE       1
#define BARRIER_RETURN_VALUE      2
#define WAIT_THREAD_RETURN_VALUE  3
#define WAIT_ALL_RETURN_VALUE    4
//Make sure int locks_returnValue is always limited to the function scope, and stack specific.
//Otherwise we might get mislead during debugging, and that won't be fun
//The purpose of these macros is legibility and debugging, although the error cases aren't likely to come up, and I don't even know how make the code robust against most of them
//locks inputLock with error printing
#define INPUT_LOCK if((locks_returnValue=pthread_mutex_lock(&inputLock))!=0){fprintf(stderr, "lock(&inputLock) failed with error %d \n",locks_returnValue); }
//unlocks inputLock with error printing
#define INPUT_UNLOCK if((locks_returnValue=pthread_mutex_unlock(&inputLock))!=0)fprintf(stderr, "unlock(&inputLock) failed with error %d \n",locks_returnValue);

#define WAIT_VALUE_LOCK if((locks_returnValue=pthread_mutex_lock(&waitCommandValueLock))!=0){fprintf(stderr, "lock(&waitCommandValueLock) failed with error %d \n",locks_returnValue);}
#define WAIT_VALUE_UNLOCK if((locks_returnValue=pthread_mutex_unlock(&waitCommandValueLock))!=0){fprintf(stderr, "unlock(&waitCommandValueLock) failed with error %d \n",locks_returnValue);}

#define THREAD_LAUNCHER_HANDLE_WAIT_ALL_COMMAND \
        if(jobsFileArgs[numThreads].waitCommandValue>0){\
          WAIT_VALUE_LOCK\
          for(i=0;i<MAX_THREADS;i++){\
            jobsFileArgs[i].timeToWait = jobsFileArgs[numThreads].waitCommandValue;\
          }\
          WAIT_VALUE_UNLOCK\
        }

#define THREAD_LAUNCHER_HANDLE_WAIT_INDEX_COMMAND \
        if((MAX_THREADS-1)>jobsFileArgs[numThreads].threadToWaitIndex){\
         if(jobsFileArgs[numThreads].waitCommandValue>0 ){\
          WAIT_VALUE_LOCK\
          jobsFileArgs[jobsFileArgs[numThreads].threadToWaitIndex].timeToWait\
          =jobsFileArgs[numThreads].waitCommandValue;\
          WAIT_VALUE_UNLOCK\
          }\
        }\
        else{\
          fprintf(stderr,"Invalid thread id: 0<=id<MAX_THREADS\n");\
        }

#define THREAD_LAUNCHER_LAUNCH_ALL_THREADS \
  while(numThreads<MAX_THREADS){\
      jobsFileArgs[numThreads].inJobFD = inJobFD ;\
      jobsFileArgs[numThreads].outJobFD= outJobFD;\
      jobsFileArgs[numThreads].tidIndex= numThreads;\
      jobsFileArgs[numThreads].timeToWait = 0 ;\
      if (pthread_create(&tid[numThreads], NULL, processJobsFile_thread, &(jobsFileArgs[numThreads])) != 0){\
          fprintf(stderr, "failed to create thread: %s\n", strerror(errno));\
          exit(EXIT_FAILURE);\
      }\
      numThreads++;\
    }\


typedef struct{
  int inJobFD;
  int outJobFD;
  int returnValue;
  unsigned int tidIndex;
  unsigned int threadToWaitIndex;
  unsigned int timeToWait;
  unsigned int waitCommandValue;
}threadArgs;

pthread_mutex_t inputLock;
pthread_mutex_t waitCommandValueLock;
unsigned int waitCommandValue;

extern pthread_rwlock_t eventListLock;
extern pthread_mutex_t ems_create_Lock;
extern pthread_mutex_t outputLock;
//flags the threads that a barrier has been encountered
//kind of a race condition, but worst case just launches up to MAX_THREADS-1 that won't do anything
int barrierEncountered=0; 

void * processJobsFile_thread(void * arg);
int writeStrToFile(char toWrite[], int fdJobOut );
int openJobsIO(char jobsDirPATH[], struct dirent * jobFile,  int * inJobFd, int * outJobFd, char * inJobPATH, char * outJobPATH );
void waitForChild(int * processCounter );
int threadJoiner(pthread_t  * tid, unsigned int numThreads, threadArgs * jobsFileArgs);
void threadLauncher(int inJobFD, int outJobFD, long unsigned int MAX_THREADS);
int childProcessLauncher( DIR * jobsDir, char * jobsDirPATH, int MAX_PROC,long unsigned int MAX_THREADS);