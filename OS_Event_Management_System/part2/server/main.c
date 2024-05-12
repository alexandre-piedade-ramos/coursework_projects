#include "main.h"

/*
Alexandre Miguel Piedade Ramos  102598 	
David Mariano Lima Rogério      107249 
*/

/*
  Correct operation may not be possible if signals other than SIGUSR1
  and SIGINT up to one time are sent.
  SIGUSR1 dumps the ems state to stdout
  SIGINT will attemp to terminate the server cleanly the first time it's sent
  and may fallback to default handling if sent again
*/
int main(int argc, char* argv[]){
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Usage: %s\n <pipe_path> [delay]\n", argv[0]);
    return 1;
  }

  char* endptr;
  unsigned int state_access_delay_us = STATE_ACCESS_DELAY_US;
  if (argc == 3) {
    unsigned long int delay = strtoul(argv[2], &endptr, 10);

    if (*endptr != '\0' || delay > UINT_MAX) {
      fprintf(stderr, "Invalid delay value or value too large\n");
      return 1;
    }

    state_access_delay_us = (unsigned int)delay;
  }

  server_pipe_path = argv[1];

  if (ems_init(state_access_delay_us)) {
    fprintf(stderr, "Failed to initialize EMS\n");
    return 1;
  }

  //Dumps the ems to stdout 
  if (signal(SIGUSR1, signal_handler)== SIG_ERR) {
    fprintf(stderr, "Failed to assign handler to SIGUSR1\n");
    exit(EXIT_FAILURE);
  }
  //Terminates the server after allowing running jobs,not queued, to finish
  if (signal(SIGINT, signal_handler)== SIG_ERR) {
    fprintf(stderr, "Failed to assign handler to SIGINT\n");
    exit(EXIT_FAILURE);
  }
  set_up_register_fifo(argv[1]);
  workerLauncher();
  

  while (true) {
    if(emsDumpPending){
      ems_dump();
      emsDumpPending=false;
    }
    if(terminateServerPending){
      terminateServer();
    }
    if(0!=registerClient()){
      perror("Failed to register Client\n");
    }
  }
  
  return 0;
}


//////////////////////////////////////////////////////////////////////////////
//Signal Handling/////////////////////////////////////////////////////////////
//Exits if signal handling is lost SIGUSR1 for some reason
//A second call to SIGINT may terminate without cleanup
static void signal_handler(int sig){
  switch(sig){
    case SIGINT:
      terminateServerPending=true;
      return;

    case  SIGUSR1:
      if (signal(SIGUSR1, signal_handler)== SIG_ERR) {
          fprintf(stderr, "Signal handler failed to reassign itself\n");
          exit(EXIT_FAILURE);
      }
      emsDumpPending=true;
      return;
    default:
      return;
  }  
}

//////////////////////////////////////////////////////////////////////////////
//sets up the request fifo
//as part of the setup for server operations, kills the server on failure
static void set_up_register_fifo(){
  if (unlink(server_pipe_path) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", server_pipe_path,
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    // Create pipe
    if (mkfifo(server_pipe_path, S_IRUSR | S_IWUSR) != 0) { 
        fprintf(stderr, "[ERR]: mkfifo failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

  /*
    quando não há escritores o conteudo da pipe que ainda não tenha
    sido lido é deitado fora, portanto vamos deixar aqui como leitor e escritor,
    e a espera do host é feita nos read em vez de no open para leitura.
  */
  server_pipe_fd = open(server_pipe_path, O_RDWR);
  if (server_pipe_fd == -1) {
      fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
  }

}

//////////////////////////////////////////////////////////////////////////////
//returns 0 on success
//will fail if SIGUSR1 is received, this is intended 
static int registerClient(){
  
  char opcode;
  char req_pipe_path[41]={'\0'};
  char resp_pipe_path[41]={'\0'};
  ssize_t ret;

  //The writes from a single message are guaranteed to be atomic up to 512
  //bytes by the posix standard. Supposedly it's actually 4096 in linux atm
  char registerRequestStream[1*sizeof(char) + 40*sizeof(char) + 40*sizeof(char) + 1];
  registerRequestStream[81]='\0';
  ret=read(server_pipe_fd, registerRequestStream, 81);
  if (ret == 0){
    fprintf(stderr, "[INFO]: pipe closed\n");
    return 1;
  }else if (ret == -1){
    fprintf(stderr, "[ERR]: read failed: %s\n", strerror(errno));
    return 1;
  }

  memcpy(&opcode, registerRequestStream, 1);
  memcpy(resp_pipe_path, &(registerRequestStream[1]), 40);
  memcpy(req_pipe_path, &(registerRequestStream[41]), 40);
  //printf("registerRequestStream: %s",registerRequestStream);
  //printf("%s %s\n",resp_pipe_path,req_pipe_path);

  //if some lock fails, just keep trying (i believe in you)
  while(0!=enqueueClient(resp_pipe_path, req_pipe_path));
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
//as part of the setup for server operations, kills the server on failure
static void  workerLauncher(){
  //All threads except the main thread must have SIGUSR1 and SIGINT blocked 
  //A copy of the caller's sigmask is inherited through pthread_create
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGINT);
  if(0!=pthread_sigmask(SIG_BLOCK, &set, NULL)){
    fprintf(stderr, "Error setting sigmask\n");
    exit(EXIT_FAILURE);
  }

  for(int i=0; i<MAX_SESSION_COUNT;i++){
    activeSessions[i].id=i;
    activeSessions[i].requestPipeFd=-1;
    activeSessions[i].responsePipeFd=-1;
    if(0!=pthread_create(&(activeSessions[i].workerThread), NULL, handleClient_thread, &(activeSessions[i]))){
      fprintf(stderr, "failed to create session handler thread: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  //main thread must have SIGUSR1
  if(0!=pthread_sigmask(SIG_UNBLOCK, &set, NULL)){
    fprintf(stderr, "Error setting sigmask\n");
    exit(EXIT_FAILURE);
  }
}

//////////////////////////////////////////////////////////////////////////////
//Worker thread function
static void * handleClient_thread(void * arg){
  Session * session = (Session *) arg;
  
  while(true){
    //if consumeClient fails on some lock, keep trying
    while(0!=consumeClient(&(session->responsePipePath), &(session->requestPipePath)));

    session->responsePipeFd=open(session->responsePipePath, O_WRONLY);
    if ((session->responsePipeFd) == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        continue;
    }
    session->requestPipeFd=open(session->requestPipePath, O_RDONLY);
    if ((session->requestPipeFd) == -1) {
        fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
        continue;
    }
    write(session->responsePipeFd, &(session->id), 1*sizeof(int));

    while(true){
      char op_code_string[2]={'\0'};
      unsigned int event_id;
      size_t num_rows, num_cols, num_seats;
      size_t * xs, * ys;
      int ret;
      bool client_quit=false;

      read(session->requestPipeFd, &op_code_string, 1*sizeof(char));
      int op_code = atoi(op_code_string);
      switch(op_code){
        case  EMS_QUIT:
          client_quit=true;
          break;
        
        case  EMS_CREATE:
          read(session->requestPipeFd, &event_id, sizeof(unsigned int));
          read(session->requestPipeFd, &num_rows, sizeof(size_t));
          read(session->requestPipeFd, &num_cols, sizeof(size_t));
          ret=ems_create(event_id, num_rows, num_cols);
          write(session->responsePipeFd, &ret, sizeof(int));
          break;
        
        case  EMS_RESERVE:
          read(session->requestPipeFd, &event_id, sizeof(unsigned int));
          read(session->requestPipeFd, &num_seats, sizeof(size_t));
          xs = (size_t *) calloc(num_seats, sizeof(size_t));
          ys = (size_t *) calloc(num_seats, sizeof(size_t));
          read(session->requestPipeFd, xs, num_seats*sizeof(size_t));
          read(session->requestPipeFd, ys, num_seats*sizeof(size_t));
          ret=ems_reserve(event_id, num_seats, xs, ys);
          write(session->responsePipeFd, &ret, sizeof(int));
          free(xs);
          free(ys);
          break;
        case  EMS_SHOW:
          read(session->requestPipeFd, &event_id, sizeof(unsigned int));
          ret=ems_show_api_resp(session->responsePipeFd,event_id);
          break;
        case  EMS_LIST:
          ret=ems_list_api_resp(session->responsePipeFd);
          break;
      }

      if(client_quit){
        free(session->requestPipePath);
        free(session->responsePipePath);
        close(session->requestPipeFd);
        close(session->responsePipeFd);
        break;
      }
    }
  }

  return NULL;
}

//////////////////////////////////////////////////////////////////////////////
//Terminate Server Cleanly - Received through SIGINT
static void terminateServer(){
  close(server_pipe_fd);
  unlink(server_pipe_path);
  ems_terminate();
  exit(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////////