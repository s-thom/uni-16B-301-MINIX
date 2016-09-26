#include <stdio.h>
#include <stdlib.h>
#include <minix/subserve.h>

#define MAX_SPACE 1024
#define NUM_ITER 204800

char space[MAX_SPACE];
struct test {
  int one;
  char two;
};

int main() {
  int success;
  int it;
  int total;
  struct test data;
  
  printf("---- TEST: Create channel ----\n");
  success = create_channel("test channel", sizeof(struct test));
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Push to full channel ----\n");
  data.one = 1337;
  data.two = 95;
  success = push("test channel", &data, MAX_SPACE);
  printf("result: %d\n", success == 1);
    
  printf("---- TEST: Close channel ----\n");
  success = close_channel("test channel");
  printf("result: %d\n", success == 1);
  
  return 0;
}





