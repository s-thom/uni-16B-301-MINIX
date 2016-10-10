#include <minix/subserve.h>
#include <stdio.h>

/* a user proccess calls create_channel when it has something it wants to send to multiple other user proccess
a channel is then created which the proccess can then input data into like a stream
*/
int create_channel(char name[], int size){
	
	int op;

	message *m = (message*) malloc(sizeof(message));
	m->m_type = SUBNUMBER;
	m->m3_i1 = CREATECHANNEL;
	m->m3_i2 = size;
	strncpy(m->m3_ca1, name, 14);
  printf("[lib] about to send\n");
	_sendrec(FS, m);
  printf("[lib] got reply, status: %d\n", m->m3_i1);
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
	m->m_type = SUBNUMBER;
	m->m3_i1 = CLOSECHANNEL;
	strncpy(m->m3_ca1, name, 14);

	_sendrec(FS, m);
	
	op = m->m3_i1;
	free(m);

	return op;
}

/* push sends the data which the data pointer is pointing to and copies it to all proccess subscripted to it that channel */
int push(char name[], void* data, size_t size){
	
	int op;

	message *m = (message*) malloc(sizeof(message));
	m->m_type = SUBNUMBER;
	m->m3_i1 = PUSHC;
	strncpy(m->m3_ca1, name, 14);
	m->m3_p1 = (char*) data;
	m->m3_i2 = size;

	_sendrec(FS, m);
	
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
	m->m_type = SUBNUMBER;
	m->m3_i1 = PULLC;
	strncpy(m->m3_ca1, name, 14);
	m->m3_p1 = (char*) data;
	m->m3_i2 = size;
		
	_sendrec(FS, m);
	
	op = m->m3_i1;
	free(m);

	return m->m3_i2;
}

/* subscribe sets it up so that the server knows you are intrested in this channel and also tells it to hold the most recent data value of this channel until
you have got around to calling pull and requesting it
*/
int subscribe(char name[]){
	int op;

	message *m = (message*) malloc(sizeof(message));
	m->m_type = SUBNUMBER;
	m->m3_i1 = SUBC;
	strcpy(m->m3_ca1, name);

	_sendrec(FS, m);
	op = m->m3_i1;
	free(m);

	return m->m3_i2;
}

/* unsubscribe removes you from the the subscribed bitmap and means the server will know longer hold the value, waiting for you */
int unsubscribe(char name[]){
	int op;

	message *m = (message*) malloc(sizeof(message));
	m->m_type = SUBNUMBER;
	m->m3_i1 = UNSUBC;
	strncpy(m->m3_ca1, name, 14);

	_sendrec(FS, m);
	op = m->m3_i1;
	free(m);

	return op;
}

int info(void){

  int op;  

  message *m = (message*) malloc(sizeof(message));
  m->m_type = SUBNUMBER;
  m->m3_i1 = INFO;
  
  _sendrec(FS, m);
	op = m->m3_i1;

  free(m);
  return op;
}



