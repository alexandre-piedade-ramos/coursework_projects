#include "api.h"

//pipe writes/reads are required by the posix standard to be atomic (no interleaving) for up to PIPE_BUF bytes
int resp_pipe_fd;
int req_pipe_fd;

int ems_setup(char const* req_pipe_path, char const* resp_pipe_path, char const* server_pipe_path) {
  if (unlink(req_pipe_path) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", req_pipe_path,
                strerror(errno));
        return 1;
    }

  if (mkfifo(req_pipe_path, S_IRUSR | S_IWUSR) != 0) { 
      fprintf(stderr, "[ERR]: mkfifo failed: %s\n", strerror(errno));
      return 1;
  }

  if (unlink(resp_pipe_path) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", resp_pipe_path,
                strerror(errno));
        return 1;
    }

  if (mkfifo(resp_pipe_path, S_IRUSR | S_IWUSR) != 0) { 
      fprintf(stderr, "[ERR]: mkfifo failed: %s\n", strerror(errno));
      return 1;
  }

  //printf("Got here just fine 1: %s\n",req_pipe_path);
  int server_pipe_fd = open(server_pipe_path, O_WRONLY);
  if (server_pipe_fd == -1) {
      fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
      return 1;
  }
  //printf("Got here just fine 2: %s\n",req_pipe_path);
  char req_pipe_path_temp[41]={'\0'};
  char resp_pipe_path_temp[41]={'\0'};

  strcpy(resp_pipe_path_temp,resp_pipe_path );
  strcpy(req_pipe_path_temp,req_pipe_path );
  char registerRequestStream[1+ 1*sizeof(char) + 40*sizeof(char) + 40*sizeof(char) ];
  registerRequestStream[81]='\0';
  memcpy(&registerRequestStream, "1", 1);
  memcpy(&(registerRequestStream[1]), resp_pipe_path_temp, 40);
  memcpy(&(registerRequestStream[41]), req_pipe_path_temp, 40);

  //printf("registerRequestStream: %s",registerRequestStream);
  write(server_pipe_fd, registerRequestStream, 81);
  close(server_pipe_fd);

  resp_pipe_fd = open(resp_pipe_path, O_RDONLY);
  if (resp_pipe_fd == -1) {
      fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
      return 1;
  }

  req_pipe_fd = open(req_pipe_path, O_WRONLY);
  if (req_pipe_fd == -1) {
      fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
      return 1;
  }

  int response;
  read(resp_pipe_fd, &response, sizeof(int));
  

  fprintf(stderr, "Server sent session id:%d\n", response);

  return 0;
}

int ems_quit(char const* req_pipe_path, char const* resp_pipe_path) { 
  //TODO: close pipes
  char op_code=EMS_QUIT;
  write(req_pipe_fd, &op_code, sizeof(char));

  close(resp_pipe_fd);
  close(req_pipe_fd);

  if (unlink(req_pipe_path) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", req_pipe_path,
                strerror(errno));
        return 1;
    }
  
  if (unlink(resp_pipe_path) != 0 && errno != ENOENT) {
        fprintf(stderr, "[ERR]: unlink(%s) failed: %s\n", resp_pipe_path,
                strerror(errno));
        return 1;
    }

  return 0;
}

int ems_create(unsigned int event_id, size_t num_rows, size_t num_cols) {
  char op_code=EMS_CREATE;
  write(req_pipe_fd, &op_code, sizeof(char));
  write(req_pipe_fd, &event_id, sizeof(unsigned int));
  write(req_pipe_fd, &num_rows, sizeof(size_t));
  write(req_pipe_fd, &num_cols, sizeof(size_t));

  int ret;
  read(resp_pipe_fd,&ret, sizeof(int) );
  //printf("Returned: %d\n", ret);
  return ret;
}

int ems_reserve(unsigned int event_id, size_t num_seats, size_t* xs, size_t* ys) {
  char op_code=EMS_RESERVE;
  write(req_pipe_fd, &op_code, sizeof(char));
  write(req_pipe_fd, &event_id, sizeof(unsigned int));
  write(req_pipe_fd, &num_seats, sizeof(size_t));
  write(req_pipe_fd, xs, num_seats*sizeof(size_t));
  write(req_pipe_fd, ys, num_seats*sizeof(size_t));

  int ret;
  read(resp_pipe_fd,&ret, sizeof(int) );
  //printf("Returned: %d\n", ret);
  return ret;
}

int ems_show(int out_fd, unsigned int event_id) {
  char op_code=EMS_SHOW;
  write(req_pipe_fd, &op_code, sizeof(char));
  write(req_pipe_fd, &event_id, sizeof(unsigned int));
  
  int ret;
  read(resp_pipe_fd, &ret, sizeof(int));
  if(ret!=0)  return ret;

  size_t num_rows;
  read(resp_pipe_fd, &num_rows, sizeof(size_t));

  size_t num_cols;
  read(resp_pipe_fd, &num_cols, sizeof(size_t));

  unsigned int * data= (unsigned int *) calloc(num_rows*num_cols, sizeof(unsigned int));
  read(resp_pipe_fd, data, num_rows*num_cols*sizeof(unsigned int));

  ems_show_printer(out_fd, num_rows, num_cols, data);
  free(data);
  return 0;
}

int ems_list_events(int out_fd) {
  //TODO: send list request to the server (through the request pipe) and wait for the response (through the response pipe)
    //TODO: send show request to the server (through the request pipe) and wait for the response (through the response pipe)
  char op_code=EMS_LIST;
  write(req_pipe_fd, &op_code, sizeof(char));
  
  int ret;
  read(resp_pipe_fd, &ret, sizeof(int));
  if(ret!=0) return ret;

  size_t num_events;
  read(resp_pipe_fd, &num_events, sizeof(size_t));
  if (num_events == 0) {
    printf("No events\n");
    return 0;
  }

  unsigned int * ids= (unsigned int *) calloc(num_events, sizeof(unsigned int));
  read(resp_pipe_fd, ids, num_events*sizeof(unsigned int));

  ems_list_printer(out_fd, num_events, ids);
  free(ids);
  return 0;
}



