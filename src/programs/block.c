#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <minix/subserve.h>

char *channel_name = "blocker";

int main(void) {
  int result = 0;
  
  printf("Block receiver sleeping to ensure channel is up\n");
  sleep(2);
  printf("Block receiver active\n");
  
  subscribe(channel_name);
  
  while (1) {
    if (pull(channel_name, &result, 1))
      printf("Message going though\n");
    else
      printf("Error in subserv\n");
    /*sleep(2);*/
  }
}
