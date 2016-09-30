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
#define INFO 6

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
int create_channel(char name[], int size);
int close_channel(char name[]);
int push(char name[], void* data, size_t size);
int pull(char name[], void*data, size_t size);
int subscribe(char name[]);
int unsubscribe(char name[]);
int info(void);



