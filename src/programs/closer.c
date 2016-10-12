#include <stdio.h>
#include <stdlib.h>
#include <minix/subserve.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int server(void);
int client(void);

char *channel_name = "closing";

int main(void){
	if (!create_channel(channel_name, sizeof(int))) {
    printf("[s] Couldn't create channel\n");
	  return 1;
	}

	if (fork()) {
	  server();
	} else {
	  client();
	}

}

int server(void) {
  int timer = 3; /* Number of iterations to run */

  printf("[s] Starting up\n");
  while (1) {
    if (!timer) {
      printf("[s] Shutting down\n");
      close_channel(channel_name);
      return 0;
    }
    
    push(channel_name, &timer, sizeof(int));
    
    timer--;
    sleep(3); /* Goodnight */
  }
}

int client(void) {
  int buff_size;
  int pull_size;
  char *storage;
  
  while (! (buff_size = subscribe(channel_name))) {
    
    printf("[c] Sleeping for 1s to wait for server");
    sleep(1);
  }
  
  /* Create a storage area of the size the server gave us */
  storage = (int *) malloc(buff_size);
  
  while (1) {
    pull_size = pull(channel_name, storage, buff_size);
    
    if (pull_size == 0) {
      printf("[c] Nothing copied! Closing down");
      return 1;
    }
    
    storage[pull_size] = '\0'; /* Explicit terminator, just in case */
    printf("[c] Value: %d\n", *storage);
    
    sleep(1);
  }
}

