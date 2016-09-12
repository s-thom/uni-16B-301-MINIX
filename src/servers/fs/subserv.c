/* Holds the main function for the subscription server
 */
 
#include "fs.h"
#include <minix/callnr.h>
#include <minix/com.h>

#include "subserv.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h>

struct channel *channels = NULL;

PUBLIC int do_subserv() {
  /* TODO: check message status code, act accordingly */
  printf("Message reached server\n");
}

struct channel* create_channel(char *name, char oid) {
  struct channel *nc = (struct channel*) malloc(sizeof(struct channel));
  int i;
  
  /* Zero the entire string, including the explicit NULL terminator */
  for (i = 0; i < 16; i++) {
    nc->name[i] = 0;
  }
  strncpy(nc->name, name, 14);
  
  nc->oid = oid;
  nc->subscribed = 0;
  nc->unreceived = 0;
  nc->waiting = 0;
  nc->content = NULL;
  nc->content_size = 0;
  nc->next = NULL;
  
  return nc;
}

struct channel* find_channel(char *name) {
  struct channel *curr = channels;
  
  while (curr != NULL) {
    if (strncmp(name, curr->name, 14) == 0) {
      break;
    }
    
    curr = curr->next;
  }
  
  return curr;
}

/* uses the index to find the bit you want to change, sets the bit to the bool value 0 or 1 and then returns */
long set_map(int index, int boolean, long current_map){
  
  long mask;  
  
  mask = 0x01;
  mask = mask << index;
  
  if(boolean){
    return current_map | mask;
  }
  else{
    mask = ~ mask;
    return current_map & mask;
  }
}

int get_map(int index, long current_map){
  long mask;

  mask = 0x01;
  mask = mask << index - 1;
  mask = ~ mask;
  current_map = current_map & mask;
  current_map = current_map >> index;
  return (int) current_map;
  
}
