#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <minix/subserve.h>

char *channel_name = "blocker";

int main(void) {
  int result = 0;
  create_channel(channel_name, 1);
  subscribe(channel_name);
  
  while (1) {
    if (pull(channel_name, &result, 1))
      printf("Subserve is not blocking\n");
    else
      printf("Error in subserv\n");
    sleep(2);
  }
}
