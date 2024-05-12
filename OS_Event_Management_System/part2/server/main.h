#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/constants.h"
#include "common/io.h"
#include "operations.h"

//our includes
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "clientQueue.h"
#define PATH_MAX 4096

enum OP_CODE {
    INVALID,
    EMS_SETUP,
    EMS_QUIT,
    EMS_CREATE,
    EMS_RESERVE,
    EMS_SHOW,
    EMS_LIST,
};


typedef struct{
    int id;
    //worker thread is responsible for freeing
    char * requestPipePath;
    int requestPipeFd;
    //worker thread is responsible for freeing
    char * responsePipePath;
    int responsePipeFd;
    pthread_t workerThread;
}Session;

static Session activeSessions[MAX_SESSION_COUNT];
static int server_pipe_fd;
static char * server_pipe_path;
//Used to notify the main thread of a SIGUSR1 request
//bool rather than counter to allow only one such request at a time
static bool emsDumpPending=false;

//Used to notify the main thread of a SIGINT request for cleanup
//A second request may be handled by the default behaviour of SIGINT
static bool terminateServerPending=false;

static void set_up_register_fifo();
static void  workerLauncher();
static void * handleClient_thread(void * arg);
static int registerClient();
static void signal_handler(int sig);
static void terminateServer();
