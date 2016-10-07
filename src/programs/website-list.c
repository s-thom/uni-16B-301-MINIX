#include <stdio.h>
#include <stdlib.h>
#include <minix/subserve.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int server(void);
int client(void);

char *channel_name = "website!";
int timer = 10; /* Number of iterations to run */

int main(void){
  pid_t proc;

	srand(time(NULL));
	
	if (create_channel(channel_name, 128)) { /* Limit URLs to 128 characters */
    printf("[s] Couldn't create channel\n");
    timer = 0;
	  return 1;
	}

	if (proc = fork()) {
	  server();
	} else {
	  client();
	}

}

int server(void) {
  char *sites[] = {
    "https://google.co.nz/",
    "https://cms.waikato.ac.nz/",
    "https://elearn.waikato.ac.nz/course/view?id=29325",
    "https://en.wikipedia.org/wiki/Main_Page",
    "https://stackoverflow.com",
    "https://xkcd.com/303",
    "http://paul.pm/301",
    "http://example.com"   
  };
  int num_sites = 8; /* Hopefully using a power of 2 will prevent most mod biases */
  int index;

  
  while (1) {
    if (!timer) {
      printf("[s] Shutting down\n");
      return 0;
    }
  
    index = rand() % num_sites;
    
    printf("[s] About to push\n");
    push(channel_name, sites[index], strlen(sites[index]));
    printf("[s] Pushed\n");
    
    sleep(3); /* Goodnight */
  }
}

int client(void) {
  int buff_size;
  int pull_size;
  char *storage;
  
  while (! (buff_size = subscribe(channel_name))) {
    if (!timer) {
      printf("[c] Shutting down\n");
      return 0;
    }
    
    printf("[c] Sleeping for 1s to wait for server");
    sleep(1);
  }
  printf("[c] SUBSCRIBED. Buffer size: %d\n", buff_size);
  /* Create a storage area of the size the server gave us */
  storage = (char *) malloc(buff_size);
  
  while (1) {
    if (!timer) {
      printf("[s] Shutting down\n");
      return 0;
    }
    
    pull_size = pull(channel_name, storage, buff_size);
    
    if (pull_size == 0) {
      printf("[c] Erroneous pull");
      return 1;
    }
    
    storage[pull_size] = '\0'; /* Explicit terminator, just in case */
    printf("[c] %s (%d)\n", storage, pull_size);
    sleep(1);
  }
}

