#include <time.h>
#include <stdlib.h>
#include <minix/subserve.h>
#include <unistd.h>


int main(){
	
	int r;
	int op;

	srand(time(NULL));

	subscribe("Rgen"); 

	while(1){
		pull("Rgen", &r, sizeof(int));
		printf("Random received %d\n", r);	

	}
}
