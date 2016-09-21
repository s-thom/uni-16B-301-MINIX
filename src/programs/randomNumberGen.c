#include <time.h>
#include <stdlib.h>
#include <minix/subserve.h>
#include <unistd.h>



int main(){
	int r;
	int op;

	srand(time(NULL));

	create_channel("Rgen", sizeof(int)); 

	while(1){
		r = rand(); 		

		op = push("Rgen", &r, sizeof(int));
		printf("Random Number %d was pushed to the channel Rgen and the opcode = %d\n", r, op);
		sleep(5);
	}

}
