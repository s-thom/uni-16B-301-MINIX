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
  
  printf("---- TEST: Close channel ----\n");
  success = close_channel("test channel");
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Close non-existent channel ----\n");
  success = close_channel("test channel");
  printf("result: %d\n", success == 0);
  
  printf("---- TEST: Reopen channel ----\n");
  success = create_channel("test channel", sizeof(struct test));
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Pull from non-subscribed channel ----\n");
  success = pull("test channel", &space, MAX_SPACE);
  printf("result: %d\n", success == 0);
  
  printf("---- TEST: Subscribe to channel ----\n");
  success = subscribe("test channel");
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Unsubscribe from channel ----\n");
  success = unsubscribe("test channel");
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Unsubscribe from non-subscribed channel ----\n");
  success = unsubscribe("test channel");
  printf("result: %d\n", success == 0);
  
  printf("---- TEST: Resubscribe to channel ----\n");
  success = subscribe("test channel");
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Subscribe to channel once more ----\n");
  success = subscribe("test channel");
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Pull from non-existent channel ----\n");
  success = pull("second channel", &space, MAX_SPACE);
  printf("result: %d\n", success == 0);
  
  printf("---- TEST: Pull from similar channel ----\n");
  success = pull("tost channel", &space, MAX_SPACE);
  printf("result: %d\n", success == 0);
  
  printf("---- TEST: Pull from empty channel ----\n");
  success = pull("test channel", &space, MAX_SPACE);
  printf("result: %d\n", success == 0);
  
  printf("---- TEST: Push to empty channel ----\n");
  data.one = 256;
  data.two = 64;
  success = push("test channel", &space, MAX_SPACE);
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Pull content ----\n");
  success = pull("test channel", &data, MAX_SPACE);
  printf("data is %d and %c\n", ((struct test*)space)->one, ((struct test*)space)->two);
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Pull content again ----\n");
  success = pull("test channel", &space, MAX_SPACE);
  printf("data is %d and %c\n", ((struct test*)space)->one, ((struct test*)space)->two);
  printf("result: %d\n", success == 0);
  
  printf("---- TEST: Push to full channel ----\n");
  data.one = 1337;
  data.two = 95;
  success = push("test channel", &data, MAX_SPACE);
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Pull content yet again ----\n");
  success = pull("test channel", &space, MAX_SPACE);
  printf("data is %d and %c\n", ((struct test*)space)->one, ((struct test*)space)->two);
  printf("result: %d\n", success == 1);

  printf("---- TEST: info ----\n");
  success = info();
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Close channel last time ----\n");
  success = close_channel("test channel");
  printf("result: %d\n", success == 1);
  
  printf("---- TEST: Pull from just closed channel ----\n");
  success = pull("test channel", &space, MAX_SPACE);
  printf("data is %d and %c\n", ((struct test*)space)->one, ((struct test*)space)->two);
  printf("result: %d\n", success == 0);
  
  printf("---- TEST: Lots of open close ----\n");
  total = 0;
  for (it = 0; it < NUM_ITER; it++) {
    total += create_channel("spam channel", sizeof(struct test));
    total += close_channel("spam channel");
  }
  printf("result: %d\n", total == (2 * NUM_ITER));
  
  printf("---- TEST: Lots of push ----\n");
  total = 0;
  create_channel("toast channel", sizeof(struct test));
  for (it = 0; it < NUM_ITER; it++) {
    total += push("toast channel", &data, MAX_SPACE);
  }
  close_channel("toast channel");
  printf("result: %d\n", total == NUM_ITER);
  
  return 0;
}





