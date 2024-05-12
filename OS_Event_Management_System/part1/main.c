#include "main.h"
unsigned int state_access_delay_ms;

/*
Projeto 1 de SO
Alexandre Miguel Piedade Ramos 102598
David Mariano Lima Rogério 107249
*/

int main(int argc, char *argv[]) {
  
  int MAX_PROC;
  long int MAX_THREADS_IN;
  long unsigned int MAX_THREADS;
  state_access_delay_ms = STATE_ACCESS_DELAY_MS;
  DIR * jobsDir;
  char jobsDirPATH[PATH_MAX];
  
  if(argc<4){
    fprintf(stderr, "Missing arguments\n");
    return 1;
  }

  if(argc>4){
    fprintf(stderr, "Too many arguments\n");
    return 1;
  }

  //args reading
  
  strcpy(jobsDirPATH, argv[1]);
    jobsDir = opendir(jobsDirPATH);
    if(jobsDir==NULL){
      fprintf(stderr, "Failed to open JOBS Dir\n");
      return 1;
    }

    sscanf(argv[2], "%u", &MAX_PROC);
    if(MAX_PROC<0){
      fprintf(stderr, "Invalid MAX_PROC\n");
      return 1;
    }

    sscanf(argv[3], "%ld", &MAX_THREADS_IN);
    if(MAX_THREADS_IN<0){
      fprintf(stderr, "Invalid MAX_THREADS\n");
      return 1;
    }
    MAX_THREADS = (long unsigned int) MAX_THREADS_IN;

    if (argc > 4) { /*argc is 1 with no args ("./ems"), changed limit from >1 to >2, for receiving jobs path*/ 
      char *endptr;
      unsigned long int delay = strtoul(jobsDirPATH, &endptr, 10);

      if (*endptr != '\0' || delay > UINT_MAX) {
        fprintf(stderr, "Invalid delay value or value too large\n");
        return 1;
      }

      state_access_delay_ms = (unsigned int)delay;
    }

  if (ems_init(state_access_delay_ms)) {
        fprintf(stderr, "Failed to initialize EMS\n");
        return 1;
  }
  int processCounter=childProcessLauncher( jobsDir, jobsDirPATH, MAX_PROC,MAX_THREADS);

  while(processCounter>0) waitForChild(&processCounter);
  
  closedir(jobsDir);
  ems_terminate();
  return 0;
}

/*
returns:
  JOB_DONE_VALUE if the .jobs is done
  BARRIER_RETURN_VALUE if BARRIER was encountered
  free the returned pointer!
  on abnormal exit relly on pthread_join return instead
*/
void * processJobsFile_thread(void * arg){
  threadArgs *args = (threadArgs *) arg;
  int jobFileFd = args->inJobFD;
  int outJobFD = args->outJobFD;
  args->returnValue =JOB_DONE_VALUE;
  int returnCheck;
  unsigned int event_id, delay;
  int locks_returnValue;
  size_t num_rows, num_columns, num_coords;
  size_t xs[MAX_RESERVATION_SIZE], ys[MAX_RESERVATION_SIZE];
  unsigned int thread_id_for_wait;

  WAIT_VALUE_LOCK;
  if(args->timeToWait>0){
    WAIT_VALUE_UNLOCK;
    ems_wait(args->timeToWait);
  }else WAIT_VALUE_UNLOCK;

  INPUT_LOCK;
  if(barrierEncountered){
    args->returnValue=BARRIER_RETURN_VALUE;
    INPUT_UNLOCK;
    return NULL;
  }
  switch (get_next(jobFileFd)) {
    case CMD_CREATE:
      if (parse_create(jobFileFd, &event_id, &num_rows, &num_columns) != 0) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        INPUT_UNLOCK;
        break;
      }
      INPUT_UNLOCK;
      if (ems_create(event_id, num_rows, num_columns)) {
        fprintf(stderr, "Failed to create event\n"); 
      }
      break;

    case CMD_RESERVE:
      num_coords = parse_reserve(jobFileFd, MAX_RESERVATION_SIZE, &event_id, xs, ys);
      INPUT_UNLOCK;

      if (num_coords == 0) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        break;;
      }
      if (ems_reserve(event_id, num_coords, xs, ys)) {
        fprintf(stderr, "Failed to reserve seats\n");
      }

      break;

    case CMD_SHOW:
      if (parse_show(jobFileFd, &event_id) != 0) {
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        INPUT_UNLOCK;
        break;
      }
      INPUT_UNLOCK;
      if (ems_show(event_id, outJobFD)) {
        fprintf(stderr, "Failed to show event\n");
      }

      break;

    case CMD_LIST_EVENTS:
      INPUT_UNLOCK;
      if (ems_list_events(outJobFD)) {
        fprintf(stderr, "Failed to list events\n");
      }

      break;

    case CMD_WAIT:
      returnCheck=parse_wait(jobFileFd, &delay, &thread_id_for_wait);
      INPUT_UNLOCK;
      if ( returnCheck== -1) {  
        fprintf(stderr, "Invalid command. See HELP for usage\n");
        break;
      }

      if(!(delay>0))  fprintf(stderr,"Invalid wait time, must be >0\n");
      args->timeToWait=delay;
      args->returnValue=WAIT_ALL_RETURN_VALUE;

      if(returnCheck==1){
        args->threadToWaitIndex=thread_id_for_wait;
        args->returnValue=WAIT_THREAD_RETURN_VALUE;
      }
      break;

    case CMD_INVALID:
      INPUT_UNLOCK;
      fprintf(stderr, "Invalid command. See HELP for usage\n");
      break;

    //asumo que isto não é para escrever no .out
    case CMD_HELP:
      INPUT_UNLOCK;
      fprintf(stderr,
          "Available commands:\n"
          "  CREATE <event_id> <num_rows> <num_columns>\n"
          "  RESERVE <event_id> [(<x1>,<y1>) (<x2>,<y2>) ...]\n"
          "  SHOW <event_id>\n"
          "  LIST\n"
          "  WAIT <delay_ms> [thread_id]\n"  // thread_id is not implemented
          "  BARRIER\n"                      // Not implemented
          "  HELP\n");

      break;

    case CMD_BARRIER:  
        barrierEncountered=1;
        INPUT_UNLOCK;
        args->returnValue=BARRIER_RETURN_VALUE;
        return NULL;

    case CMD_EMPTY:
      INPUT_UNLOCK;
      break;

    case EOC:
      INPUT_UNLOCK;
      args->returnValue= JOB_FILE_DONE_VALUE;
      break;
  }
  
  return NULL;
}

/*
returns -1 if an error was encountered
returns 0 if IO is ready
returns 1 if file was ignored (no files were opened)
*/
int openJobsIO(char jobsDirPATH[], struct dirent * jobFile,  int * inJobFd, int * outJobFd, char * inJobPATH, char * outJobPATH ){

    if(strlen(jobFile->d_name)<strlen(JOB_FILE_EXTENSION)) return 1;
    size_t jobFileExtLen = strlen(jobFile->d_name) - strlen(JOB_FILE_EXTENSION);
    //if not a JOB_FILE_EXTENSION file ignore
    if(0!=strcmp(JOB_FILE_EXTENSION, jobFile->d_name + sizeof(char)*(jobFileExtLen)))
      return 1;
    
    strcpy(inJobPATH, jobsDirPATH);
    strcat(inJobPATH, "/");
    strcat(inJobPATH,jobFile->d_name);

    *inJobFd = open(inJobPATH, O_RDONLY);
    if (*inJobFd < 0){
      fprintf(stderr, "open error: %s\n", strerror(errno));
      return -1;
    }

    strcpy(outJobPATH, jobsDirPATH);
    strcat(outJobPATH, "/");
    strcat(outJobPATH,jobFile->d_name);
    strcpy(
          outJobPATH + sizeof(char)*(strlen(outJobPATH)- strlen(JOB_FILE_EXTENSION)), 
          JOB_OUTPUT_EXTENSION
          );
    *outJobFd = open(outJobPATH, O_CREAT | O_TRUNC | O_WRONLY , S_IRUSR | S_IWUSR);
    if (*outJobFd < 0){
        fprintf(stderr, "open error: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

//waits for a child process to be complete, and prints the approriate message to stdout
void waitForChild(int * processCounter ){
    int processStatus;
    int pID;
    pID=wait(&processStatus);
    (*processCounter)--;
    fprintf(stdout, "Child Process PID=%d ",pID);

    if(WIFEXITED(processStatus)){
      fprintf(stdout, "terminated normally with status %d ",WEXITSTATUS(processStatus));
      if(WEXITSTATUS(processStatus)==FILE_IGNORED_STATUS)
        //will always print at least twice because of . and ..
        fprintf(stdout,". A file in jobs Dir wasn't a %s file. ", JOB_FILE_EXTENSION);
    }
    else if(WIFSIGNALED(processStatus))
      fprintf(stdout, "was terminated by signal : %d with status %d",WTERMSIG(processStatus), WEXITSTATUS(processStatus));
    else //might want to handle sigstop separately, for now this will do
      fprintf(stdout, "behaved abnormally with status %d", WEXITSTATUS(processStatus));

    fprintf(stdout,"\n");
} 



int threadJoiner(pthread_t  * tid, unsigned int numThreads, threadArgs * jobsFileArgs){
    int checkForError,checkThreadReturn;
    checkForError=pthread_join(tid[numThreads-1], NULL);
    checkThreadReturn=jobsFileArgs[numThreads-1].returnValue;
    if(checkForError!=0){
      fprintf(stderr, "pthread_join for thread id %d failed with error %d\n",numThreads,checkForError);
      ems_terminate(); //heap is per child process, and events don't overlap between processes
      exit(EXIT_FAILURE);
    }
    
    return checkThreadReturn;
}

//returns number of still running child processes
int childProcessLauncher( DIR * jobsDir, char * jobsDirPATH, int MAX_PROC,long unsigned int MAX_THREADS){
  int checkReturn;
  char inJobPATH[PATH_MAX],outJobPATH[PATH_MAX];
  int inJobFD, outJobFD;
  int pID; int processCounter=0;
  struct dirent * jobFile;
  
  while(1){
    errno=0; //required for error checking with readdir
    jobFile = readdir(jobsDir);
    if(jobFile==NULL && errno!=0){
      fprintf(stderr, "Failed to read file in jobs dir\n");
      return 1;
    }
    
    if(jobFile == NULL && errno==0)
      //All files in jobs dir read
      break;

    if(!strcmp(jobFile->d_name,".")||!strcmp(jobFile->d_name,"..") )
      continue;

    pID=fork();
    if(pID<0){
      fprintf(stderr, "Failed to fork process: %s\n", strerror(errno));
      return 1;
    }
    if(pID==0){
      checkReturn=openJobsIO(jobsDirPATH,jobFile,&inJobFD,&outJobFD,inJobPATH,outJobPATH);

      //Some error was encountered in opening the files
      if(checkReturn<0)exit(EXIT_FAILURE);
      //The file this process was meant to handle was not a .jobs file. This is considered successful execution. 
      if(checkReturn>0){
        exit(FILE_IGNORED_STATUS);
      }

      if(pthread_mutex_init(&inputLock, NULL)!=0) fprintf(stderr, "mutex init failed\n");
      if(pthread_mutex_init(&waitCommandValueLock, NULL))fprintf(stderr, "mutex init failed\n");
      if(pthread_mutex_init(&ems_create_Lock, NULL)!=0) fprintf(stderr, "mutex init failed\n");
      if(pthread_mutex_init(&outputLock, NULL)!=0) fprintf(stderr, "mutex init failed\n");
      if(pthread_rwlock_init(&eventListLock,NULL)!=0)fprintf(stderr, "rwlock init failed\n");

      
      //cook up a batch of threads
      threadLauncher(inJobFD,outJobFD , MAX_THREADS);
      
      ems_terminate();

      if(close(inJobFD)){
        fprintf(stderr, "Failed to close file in jobs dir\n");
        exit(EXIT_FAILURE);
      }

      if(close(outJobFD)){
        fprintf(stderr, "Failed to close file in jobs dir\n");
        exit(EXIT_FAILURE);
      }
      //exit argument must be integer between 0 and 255
      exit(EXIT_SUCCESS);
    }
    if(pID>0){
      processCounter++;
      if(processCounter<MAX_PROC) continue;
      //else wait for running process to conclude before forking again
      waitForChild(&processCounter);
    }
  }

  return processCounter;
}

void threadLauncher(int inJobFD, int outJobFD, long unsigned int MAX_THREADS){
  int checkThreadReturn, fileDoneFlag=0, locks_returnValue;
  pthread_t  * tid = (pthread_t *) malloc(MAX_THREADS * sizeof(pthread_t));
  threadArgs * jobsFileArgs= (threadArgs*) malloc(MAX_THREADS * sizeof(threadArgs));
  unsigned int numThreads=0,i;

  //cook up a batch
  THREAD_LAUNCHER_LAUNCH_ALL_THREADS
  
  while(1){
    //for each thread that returns another is created
    checkThreadReturn=threadJoiner(tid,numThreads,jobsFileArgs);
    numThreads--;
    if(checkThreadReturn==JOB_FILE_DONE_VALUE){
      while(numThreads>0){
        checkThreadReturn=threadJoiner(tid, numThreads,jobsFileArgs);
        numThreads--;
      }
      break;
    }
    else if(checkThreadReturn==BARRIER_RETURN_VALUE){
      //leave one for the outer loop
      while(numThreads>0){
        checkThreadReturn=threadJoiner(tid, numThreads,jobsFileArgs);
        numThreads--;
        if(checkThreadReturn==JOB_FILE_DONE_VALUE) fileDoneFlag=1;
        else if(checkThreadReturn==WAIT_ALL_RETURN_VALUE){
          THREAD_LAUNCHER_HANDLE_WAIT_ALL_COMMAND;
        }
        else if(checkThreadReturn==WAIT_THREAD_RETURN_VALUE){
          THREAD_LAUNCHER_HANDLE_WAIT_INDEX_COMMAND;
        }
      }
      barrierEncountered=0;
      if(fileDoneFlag) break;
      //cook up another batch
      THREAD_LAUNCHER_LAUNCH_ALL_THREADS
    }
    else if(checkThreadReturn==JOB_DONE_VALUE){
      jobsFileArgs[numThreads].tidIndex= numThreads;
      if (0!=pthread_create(&tid[numThreads], NULL, processJobsFile_thread, &(jobsFileArgs[numThreads]))) {
          fprintf(stderr, "failed to create thread: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
      }
      numThreads++;
    }
    else if(checkThreadReturn==WAIT_ALL_RETURN_VALUE){
      THREAD_LAUNCHER_HANDLE_WAIT_ALL_COMMAND;
      jobsFileArgs[numThreads].tidIndex= numThreads;
      if (0!=pthread_create(&tid[numThreads], NULL, processJobsFile_thread, &(jobsFileArgs[numThreads]))) {
          fprintf(stderr, "failed to create thread: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
      }
      numThreads++;
    }
    else if(checkThreadReturn==WAIT_THREAD_RETURN_VALUE){
      THREAD_LAUNCHER_HANDLE_WAIT_INDEX_COMMAND;
      jobsFileArgs[numThreads].tidIndex= numThreads;
      if (0!=pthread_create(&tid[numThreads], NULL, processJobsFile_thread, &(jobsFileArgs[numThreads]))) {
          fprintf(stderr, "failed to create thread: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
      }
      numThreads++;
    }
    else{
      fprintf(stderr,"Abnormal thread return value\n");
    }
  }
  free(tid);
  free(jobsFileArgs);
}


