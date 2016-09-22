#include <time.h>
#include <stdlib.h>
#include <minix/subserve.h>
#include <unistd.h>

int op = -1;
char array[100];
int pullSize = 0;
int pushSize = sizeof(int);





int main(){
  int i = 10000;  

  op = subscribe("test");
  printf("Test: subscibe to channel that doesnt exist\nResult = %d\n", op);
  op = unsubscribe("test");
  printf("Test: Unsubscibe to channel that doesnt exist\nResult = %d\n", op);
  op = push("test", &array, pullSize);
  printf("Test: pushing to channel that doesnt exist\nResult = %d\n", op);
  
  printf("creating and removing channels:\n");
  /* creating and removing a channel */
  while(i > 0){
    pushSize = sizeof(int);
    op = create_channel("test", pushSize);
    if(op != 1) printf("Channel was not created correctly\n");
    pullSize = subscribe("test");
    if(pullSize != 1) printf("ERROR: subscribe failed");
    /* if(pullSize != pushSize) printf("Pull size didnt equal push. Error!!!\n"); */    
    op = close_channel("test");
    if(op != 1) printf("Channel Did not close correctly\n");
    op = subscribe("test");
    if(op == 1) printf("ERROR: subscribed to closed channel");

    i--;
  }
  printf("finished\n");

  /* transfer array of date */
  op = create_channel("test", 100);
  
  strcpy(array, "Hello Jayden and Stuart, You have managed to tranfer this string to and from the fs!! :)");
  op = push("test", &array, 100);
  op = subscribe("test");
  memset(array, 0, 100);
  op = pull("test", &array, 100);
  printf("%s\n", array);

  

}
