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
  printf("Block receiver subscribed\n");
  
  while (result < 5) {
    if (pull(channel_name, &result, sizeof(int)))
      printf("Result: %d\n", result);
    else{
      printf("Error in subserv\n");
      return;}
    /*sleep(2);*/
  }
  
  printf("Block receiver shutting down\n");
  unsubscribe(channel_name);
  return 0;
}
