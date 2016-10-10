#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <minix/subserve.h>

char *channel_name = "blocker";

int main(void) {
  int result = 0;
  int status;
  status = create_channel(channel_name, 1);
  if (!status) {
    printf("[s] create channel failed\n");
    return 1;
  }
  printf("[s] channel created\n");
  
  while (result < 7) {
    if (push(channel_name, &result, sizeof(int))){
      printf("[s] Successfully pushed\n");
      result++;}
    else
      printf("[s] Error in subserv\n");
    sleep(2);
  }
  printf("[s] Block server shutting down\n");
  close_channel(channel_name);
  return 0;
}
