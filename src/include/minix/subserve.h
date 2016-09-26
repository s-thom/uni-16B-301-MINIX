#include <lib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CREATECHANNEL 0
#define CLOSECHANNEL 1
#define PUSHC 2
#define PULLC 3
#define SUBC 4
#define UNSUBC 5
#define SUBNUMBER 69
/*
This file is the library that must be included by all programs wishing to use the subserve server
number in table = 69

0 = create_channel
1 = close_channal
2 = push
3 = pull
4 = subscribe
5 = unsubscribe

*/

/* a user proccess calls create_channel when it has something it wants to send to multiple other user proccess
a channel is then created which the proccess can then input data into like a stream
*/

int subserv_pn = -1;

int get_subserv_procnr(void) {
  char *name;
  message *m;
  
  printf("about to ask for ss's number\n");
  
  name = "subserv";
  m = (message*) malloc(sizeof(message));
	m->m_type = 80;
  m->m1_p1 = name;
  m->m1_i1 = -1;
  m->m1_i2 = 7;
  
	_sendrec(MM, m);
	
	subserv_pn = m->m3_i1;
	
	printf("subserv's pn is %d\n", subserv_pn);
	return subserv_pn;
}

int create_channel(char name[], int size){
	int op;
	message *m = (message*) malloc(sizeof(message));
	
	if (subserv_pn <= 0) {
	  get_subserv_procnr();
	}
	
	m->m_type = SUBNUMBER;
	m->m3_i1 = CREATECHANNEL;
	m->m3_i2 = size;
	strncpy(m->m3_ca1, name, 14);

	_sendrec(subserv_pn, m);
	op = m->m3_i1;
	free(m);

	return op;
}


/* if a user proccess which created a channel has nothing else to send or is closing it is responsible for closing the channel and thus freeing
thus resources on the subcription server
*/
int close_channel(char name[]){
	int op;
	message *m = (message*) malloc(sizeof(message));
	
	if (subserv_pn <= 0) {
	  get_subserv_procnr();
	}
	
	m->m_type = SUBNUMBER;
	m->m3_i1 = CLOSECHANNEL;
	strncpy(m->m3_ca1, name, 14);

	_sendrec(subserv_pn, m);
	
	op = m->m3_i1;
	free(m);

	return op;
}

/* push sends the data which the data pointer is pointing to and copies it to all proccess subscripted to it that channel */
int push(char name[], void* data, size_t size){
	int op;
	message *m = (message*) malloc(sizeof(message));
	
	if (subserv_pn <= 0) {
	  get_subserv_procnr();
	}
	
	m->m_type = SUBNUMBER;
	m->m3_i1 = PUSHC;
	strncpy(m->m3_ca1, name, 14);
	m->m3_p1 = (char*) data;
	m->m3_i2 = size;

	_sendrec(subserv_pn, m);
	
	op = m->m3_i1;
	free(m);

	return op;
}

/* pull is a request that you want the most recent data that was pushed to the channel and that data will be copied into the data.
size specifies the max amount of memory you have for data to be sent to you
*/
int pull(char name[], void*data, size_t size){
	int op;
	message *m = (message*) malloc(sizeof(message));
	
	if (subserv_pn <= 0) {
	  get_subserv_procnr();
	}
	
	m->m_type = SUBNUMBER;
	m->m3_i1 = PULLC;
	strncpy(m->m3_ca1, name, 14);
	m->m3_p1 = (char*) data;
	m->m3_i2 = size;
		
	_sendrec(subserv_pn, m);
	
	op = m->m3_i1;
	free(m);

	return op;
}

/* subscribe sets it up so that the server knows you are intrested in this channel and also tells it to hold the most recent data value of this channel until
you have got around to calling pull and requesting it
*/
int subscribe(char name[]){
	int op;
	message *m = (message*) malloc(sizeof(message));
	
	if (subserv_pn <= 0) {
	  get_subserv_procnr();
	}
	
	m->m_type = SUBNUMBER;
	m->m3_i1 = SUBC;
	strncpy(m->m3_ca1, name, 14);

	_sendrec(subserv_pn, m);
	op = m->m3_i1;
	free(m);

	return m->m3_i2;
}

/* unsubscribe removes you from the the subscribed bitmap and means the server will know longer hold the value, waiting for you */
int unsubscribe(char name[]){
	int op;
	message *m = (message*) malloc(sizeof(message));
	
	if (subserv_pn <= 0) {
	  get_subserv_procnr();
	}
	
	m->m_type = SUBNUMBER;
	m->m3_i1 = UNSUBC;
	strncpy(m->m3_ca1, name, 14);

	_sendrec(subserv_pn, m);
	op = m->m3_i1;
	free(m);

	return op;
}



