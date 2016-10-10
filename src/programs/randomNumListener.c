#include <time.h>
#include <stdlib.h>
#include <minix/subserve.h>
#include <unistd.h>


int main(){
	
	int r = 0;
	int op;
	int active = 0;

	srand(time(NULL));

  while (!active) {
	  active = subscribe("Rgen"); 
    sleep(1);
  }

	while(1){
		op = pull("Rgen", &r, sizeof(int));
		printf("Random received %d, opcode %d\n", r, op);	
	sleep(2);
    r = 0;

	}
}
