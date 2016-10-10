#include <stdio.h>
#include <stdlib.h>
#include <minix/subserve.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int server(void);
int client(void);

char *channel_name = "website!";
char *desc = "----- WEBSITE LIST -----\nSets up a server to push a link to a website periodically\nAlso starts a client to listen for new sites and print them out\nLines stating with [c] are from the client, [s] from the server\n------------------------\n";

int main(void){
  pid_t proc;

	srand(time(NULL));
	
	printf(desc);
	
	sleep(1);
	
	if (!create_channel(channel_name, 128)) { /* Limit URLs to 128 characters */
    printf("[s] Couldn't create channel\n");
	  return 1;
	}

	if (proc = fork()) {
	  server();
	} else {
	  client();
	}

}

int server(void) {
  int timer = 3; /* Number of iterations to run */
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

  printf("[s] Starting up\n");
  while (1) {
    if (!timer) {
      printf("[s] Shutting down\n");
      close_channel(channel_name);
      return 0;
    }
  
    index = rand() % num_sites;
    
    push(channel_name, sites[index], strlen(sites[index]));
    
    timer--;
    sleep(3); /* Goodnight */
  }
}

int client(void) {
  int buff_size;
  int pull_size;
  char *storage;
  int timer = 3; /* Number of iterations to run */
  
  while (! (buff_size = subscribe(channel_name))) {
    if (!timer) {
      printf("[c] Shutting down\n");
      return 0;
    }
    
    printf("[c] Sleeping for 1s to wait for server");
    sleep(1);
  }
  
  /* Create a storage area of the size the server gave us */
  storage = (char *) malloc(buff_size);
  
  while (1) {
    if (!timer) {
      printf("[c] Shutting down\n");
      unsubscribe(channel_name);
      return 0;
    }
    
    pull_size = pull(channel_name, storage, buff_size);
    
    if (pull_size == 0) {
      printf("[c] Erroneous pull");
      unsubscribe(channel_name);
      return 1;
    }
    
    storage[pull_size] = '\0'; /* Explicit terminator, just in case */
    printf("[c] NEW SITE: %s\n", storage);
    
    timer--;
    sleep(1);
  }
}

