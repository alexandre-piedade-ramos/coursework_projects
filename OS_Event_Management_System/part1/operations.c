#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "eventlist.h"
#include "operations.h"
#include <unistd.h>
#define EMS_CREATE_LOCK if((locks_returnValue=pthread_mutex_lock(&ems_create_Lock))!=0)fprintf(stderr, "lock(&ems_create_Lock) failed with error %d \n",locks_returnValue);  
#define EMS_CREATE_UNLOCK if((locks_returnValue=pthread_mutex_unlock(&ems_create_Lock))!=0)fprintf(stderr, "unlock(&ems_create_Lock) failed with error %d \n",locks_returnValue);  

#define EVENT_LIST_RDLOCK if((locks_returnValue=pthread_rwlock_rdlock(&eventListLock))!=0)fprintf(stderr, "rdlock(&eventListLock) failed with error %d \n",locks_returnValue);  
#define EVENT_LIST_WRLOCK if((locks_returnValue=pthread_rwlock_wrlock(&eventListLock))!=0)fprintf(stderr, "wrlock(&eventListLock) failed with error %d \n",locks_returnValue);  
#define EVENT_LIST_UNLOCK if((locks_returnValue=pthread_rwlock_unlock(&eventListLock))!=0)fprintf(stderr, "unlock(&eventListLock) failed with error %d \n",locks_returnValue);  

//locks outputLock with error printing
#define OUTPUT_LOCK  if((locks_returnValue=pthread_mutex_lock(&outputLock))!=0)fprintf(stderr, "lock(&outputLock) failed with error %d \n",locks_returnValue); 
//unlocks outputLock with error printing
#define OUTPUT_UNLOCK if((locks_returnValue=pthread_mutex_unlock(&outputLock))!=0)fprintf(stderr, "unlock(&outputLock) failed with error %d \n",locks_returnValue);  
//it takes at most 10 chars to write out a long unsigned int 
#define MAX_CHARS_LUINT 10
pthread_rwlock_t eventListLock; //= PTHREAD_RWLOCK_INITIALIZER;
pthread_mutex_t ems_create_Lock; //= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t outputLock; //= PTHREAD_MUTEX_INITIALIZER;

static struct EventList* event_list = NULL;
static unsigned int state_access_delay_ms = 0;


/// Calculates a timespec from a delay in milliseconds.
/// @param delay_ms Delay in milliseconds.
/// @return Timespec with the given delay.
static struct timespec delay_to_timespec(unsigned int delay_ms) {
  return (struct timespec){delay_ms / 1000, (delay_ms % 1000) * 1000000};
}

/// Gets the event with the given ID from the state.
/// @note Will wait to simulate a real system accessing a costly memory resource.
/// @param event_id The ID of the event to get.
/// @return Pointer to the event if found, NULL otherwise.
static struct Event* get_event_with_delay(unsigned int event_id) {
  struct timespec delay = delay_to_timespec(state_access_delay_ms);
  nanosleep(&delay, NULL);  // Should not be removed

  return get_event(event_list, event_id);
}

/// Gets the seat with the given index from the state.
/// @note Will wait to simulate a real system accessing a costly memory resource.
/// @param event Event to get the seat from.
/// @param index Index of the seat to get.
/// @return Pointer to the seat.
static unsigned int* get_seat_with_delay(struct Event* event, size_t index) {
  struct timespec delay = delay_to_timespec(state_access_delay_ms);
  nanosleep(&delay, NULL);  // Should not be removed

  return &event->data[index];
}

/// Gets the index of a seat.
/// @note This function assumes that the seat exists.
/// @param event Event to get the seat index from.
/// @param row Row of the seat.
/// @param col Column of the seat.
/// @return Index of the seat.
static size_t seat_index(struct Event* event, size_t row, size_t col) { return (row - 1) * event->cols + col - 1; }

int ems_init(unsigned int delay_ms) {
  if (event_list != NULL) {
    fprintf(stderr, "EMS state has already been initialized\n");
    return 1;
  }

  event_list = create_list();
  state_access_delay_ms = delay_ms;

  return event_list == NULL;
}

int ems_terminate() {
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  free_list(event_list);
  return 0;
}

int ems_create(unsigned int event_id, size_t num_rows, size_t num_cols) {
  int locks_returnValue;
  //RACE CONDITION LIKELY IF MORE COMMANDS ARE ADDED TO THE SPEC
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  EMS_CREATE_LOCK;
  EVENT_LIST_RDLOCK;
  
  //if the event doesn't yet exist, that'll no longer be the case
  //only just before ems_create returns 
  if (get_event_with_delay(event_id) != NULL) {
    //fprintf(stderr, "Event already exists\n");
    return 1;
  }

  EVENT_LIST_UNLOCK;

  struct Event* event = malloc(sizeof(struct Event));

  if (event == NULL) {
    fprintf(stderr, "Error allocating memory for event\n");
    EMS_CREATE_UNLOCK;
    return 1;
  }

  event->id = event_id;
  event->rows = num_rows;
  event->cols = num_cols;
  event->reservations = 0;
  event->data = malloc(num_rows * num_cols * sizeof(unsigned int));

  if(pthread_rwlock_init(&(event->eventLock), NULL)!=0){
    fprintf(stderr, "Error in pthread_rwlock_init for event %d\n", event_id);
    free(event);
    EMS_CREATE_UNLOCK;
    return 1;
  }

  if (event->data == NULL) {
    fprintf(stderr, "Error allocating memory for event data\n");
    free(event);
    EMS_CREATE_UNLOCK;
    return 1;
  }

  for (size_t i = 0; i < num_rows * num_cols; i++) {
    event->data[i] = 0;
  }

  EVENT_LIST_WRLOCK;
  if (append_to_list(event_list, event) != 0) {
    fprintf(stderr, "Error appending event to list\n");
    free(event->data);
    free(event);
    EMS_CREATE_UNLOCK;
    EVENT_LIST_UNLOCK;
    return 1;
  }
  EMS_CREATE_UNLOCK;
  EVENT_LIST_UNLOCK;
  return 0;
}

int ems_reserve(unsigned int event_id, size_t num_seats, size_t* xs, size_t* ys) {
  int locks_returnValue;
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  EVENT_LIST_RDLOCK;
  struct Event* event = get_event_with_delay(event_id);
  EVENT_LIST_UNLOCK;
  if (event == NULL) {
    //fprintf(stderr, "Event %d not found\n",event_id);
    return 1;
  }

  pthread_rwlock_wrlock(&(event->eventLock));

  

  unsigned int reservation_id = ++event->reservations;

  size_t i = 0;
  for (; i < num_seats; i++) {
    size_t row = xs[i];
    size_t col = ys[i];

    if (row <= 0 || row > event->rows || col <= 0 || col > event->cols) {
      //fprintf(stderr, "Invalid seat\n");
      break;
    }

    if (*get_seat_with_delay(event, seat_index(event, row, col)) != 0) {
      //fprintf(stderr, "Seat already reserved\n");
      break;
    }

    //cuidado que isto está a escrever no evento que o assento está reservado
    *get_seat_with_delay(event, seat_index(event, row, col)) = reservation_id;

  }

  // If the reservation was not successful, free the seats that were reserved.
  if (i < num_seats) {
    event->reservations--;
    for (size_t j = 0; j < i; j++) {
      *get_seat_with_delay(event, seat_index(event, xs[j], ys[j])) = 0;
    }
    pthread_rwlock_unlock(&(event->eventLock));
    return 1;
  }

  pthread_rwlock_unlock(&(event->eventLock));
  return 0;
}

int ems_show(unsigned int event_id, int fdJobOut) {
  long unsigned int toWriteSz=128,charNum=1;
  char toWrite[toWriteSz]; 
  int locks_returnValue;
  long unsigned event_cols;
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  EVENT_LIST_RDLOCK;

  struct Event* event = get_event_with_delay(event_id);
  
  EVENT_LIST_UNLOCK;

  if (event == NULL) {
    //fprintf(stderr, "Event not found\n");
    return 1;
  }

  pthread_rwlock_rdlock(&(event->eventLock));

  for (size_t i = 1; i <= event->rows; i++) {
    for (size_t j = 1; j <= event->cols; j++) {
      unsigned int* seat = get_seat_with_delay(event, seat_index(event, i, j));
      event_cols=event->cols;

      
      if((toWriteSz - charNum)< (MAX_CHARS_LUINT + 1) ){
        //inner loop writes at most MAX_CHARS_LUINT + (' ' or '\n')
        pthread_rwlock_unlock(&(event->eventLock));
        writeStrToFile(toWrite, fdJobOut);
        pthread_rwlock_rdlock(&(event->eventLock));
        charNum=1;
      }

      charNum+= (long unsigned int) sprintf(toWrite+(charNum-1)/**sizeof(char)*/,"%u", *seat);

      if (j < event_cols) {
        charNum+= (long unsigned int)sprintf(toWrite+(charNum-1)/**sizeof(char)*/," ");
      }
    }
    charNum+= (long unsigned int)sprintf(toWrite+(charNum-1)*sizeof(char), "\n");
  }

  pthread_rwlock_unlock(&(event->eventLock));

  if(charNum>1)
      writeStrToFile(toWrite, fdJobOut);

  return 0;
}

int ems_list_events(int fdJobOut) {
  char toWrite[128];
  int locks_returnValue;

  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  EVENT_LIST_RDLOCK;
  
  if (event_list->head == NULL) {
    EVENT_LIST_UNLOCK;
    sprintf(toWrite,"No events\n");
    writeStrToFile(toWrite, fdJobOut);
    return 0;
  }

  unsigned int current_event_id;
  struct ListNode* current = event_list->head;
  while (current != NULL) {
    current_event_id=(current->event)->id;
    EVENT_LIST_UNLOCK;
    sprintf(toWrite,"Event: %u\n", current_event_id);
    writeStrToFile(toWrite, fdJobOut);
    EVENT_LIST_RDLOCK;
    current = current->next;
  }
  EVENT_LIST_UNLOCK;
  return 0;
}

void ems_wait(unsigned int delay_ms) {
  struct timespec delay = delay_to_timespec(delay_ms);
  nanosleep(&delay, NULL);
}


int writeStrToFile(char toWrite[], int outJobFD ){
  long unsigned int i=0;
  int locks_returnValue;
  OUTPUT_LOCK;
  for(i=0;toWrite[i]!='\0';i++);
  size_t len = i;
  long int done = 0;
  while (len > 0) {
    long int bytes_written = write(outJobFD, toWrite + done, len);
    if (bytes_written < 0){
        OUTPUT_UNLOCK;
        fprintf(stderr, "write error\n");
        return -1;
    }

    /* might not have managed to write all, len becomes what remains */
    len -=  (size_t) bytes_written;
    done += bytes_written;
  }
  OUTPUT_UNLOCK;
  return 0;
}
