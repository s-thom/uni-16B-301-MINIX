#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <minix/subserve.h>
#include <time.h>

int main(){
  create_channel("test", sizeof(int));
  create_channel("test1", sizeof(int));
  create_channel("test2", sizeof(int));
  info();  
}
