#include "io.h"

int parse_uint(int fd, unsigned int *value, char *next) {
  char buf[16];

  int i = 0;
  while (1) {
    ssize_t read_bytes = read(fd, buf + i, 1);
    if (read_bytes == -1) {
      return 1;
    } else if (read_bytes == 0) {
      *next = '\0';
      break;
    }

    *next = buf[i];

    if (buf[i] > '9' || buf[i] < '0') {
      buf[i] = '\0';
      break;
    }

    i++;
  }

  unsigned long ul = strtoul(buf, NULL, 10);

  if (ul > UINT_MAX) {
    return 1;
  }

  *value = (unsigned int)ul;

  return 0;
}

int print_uint(int fd, unsigned int value) {
  char buffer[16];
  size_t i = 16;

  for (; value > 0; value /= 10) {
    buffer[--i] = '0' + (char)(value % 10);
  }

  if (i == 16) {
    buffer[--i] = '0';
  }

  while (i < 16) {
    ssize_t written = write(fd, buffer + i, 16 - i);
    if (written == -1) {
      return 1;
    }

    i += (size_t)written;
  }

  return 0;
}

int print_str(int fd, const char *str) {
  size_t len = strlen(str);
  while (len > 0) {
    ssize_t written = write(fd, str, len);
    if (written == -1) {
      return 1;
    }

    str += (size_t)written;
    len -= (size_t)written;
  }

  return 0;
}

int ems_show_printer(int out_fd, size_t rows, size_t cols, unsigned int * data) {
  for (size_t i = 1; i <= rows; i++) {
    for (size_t j = 1; j <= cols; j++) {
      char buffer[16];
      sprintf(buffer, "%u", data[((i - 1) * cols + j - 1)]);

      if (print_str(out_fd, buffer)) {
        perror("Error writing to file descriptor");
        return 1;
      }

      if (j < cols) {
        if (print_str(out_fd, " ")) {
          perror("Error writing to file descriptor");
          return 1;
        }
      }
    }

    if (print_str(out_fd, "\n")) {
      perror("Error writing to file descriptor");
      return 1;
    }
  }
  return 0;
}


int ems_list_printer(int out_fd, size_t num_events, unsigned int ids[]){
  for(size_t i=0; i<num_events; i++ ){
    char buffer[128];
    sprintf(buffer, "Event: %u\n", ids[i]);
    if (print_str(out_fd, buffer)) {
        perror("Error writing to file descriptor");
        return 1;
    }
  }
  return 0;
}