/* Holds the main function for the subscription server
 */
 
#include "fs.h"
#include <minix/callnr.h>
#include <minix/com.h>

#include "subserv.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "subserveList.c"
#include <stdio.h>

struct channel *channels = NULL;

PUBLIC int do_subserv() {
  /* TODO: check message status code, act accordingly */
  int retcode;
  char index;
  char terminated = 0;
  
  /* Zero the rest of the name as soon as a \0 is found */
  for (index = 0; index < 15; index++) {
    if (terminated) {
      m_in.m3_ca1[index] = 0;
    } else {
      if (m_in.m3_ca1[index] == 0) {
        terminated = 1;
      }
    }
  }
  
  switch(m_in.m3_i1) {
    /* CREATE */
    case 0:
      retcode = handle_create();
      break;
    /* CLOSE */
    case 1:
      retcode = handle_close();
      break;
    /* PUSH */
    case 2:
      retcode = handle_push();
      break;
    /* PULL */
    case 3:
      retcode = handle_pull();
      break;
    /* SUBSCRIBE */
    case 4:
      retcode = handle_subscribe();
      break;
    /* UNSUBSCRIBE */
    case 5:
      retcode = handle_unsubscribe();
      break;
  }
}

/**
 * Handles the creation of a channel
 */
int handle_create() {
  printf("[subserv] got CREATE\n");
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles the closing of a channel
 */
int handle_close() {
  printf("[subserv] got CLOSE\n");
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles pushing to a channel
 */
int handle_push() {
  printf("[subserv] got PUSH\n");
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles pulling from a channel
 */
int handle_pull() {
  printf("[subserv] got PULL\n");
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles subscribing to a channel
 */
int handle_subscribe() {
  printf("[subserv] got SUBSCRIBE\n");
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles unsubscription from a channel
 */
int handle_unsubscribe() {
  printf("[subserv] got UNSUBSCRIBE\n");
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
  /* m_in */

  CHANNEL *temp = get_channel(m_in.ss_name, channels);
  int sender = m_in.m_source  
  
  /* checks to see if that channel is in the list */
  if(temp != NULL){
    
    if(get_map(sender, temp->subscribed)){
     /* sets sub and unrec to 0 those removing it from the bitmap */
     temp->subscribed = set_map(sender, 0, temp->subscribed);
     temp->unreceived = set_map(sender, 0, temp->unreceived); 
    }
    else{
      /* its trying to unsubscribe from channel its not subed 2 */
      return SS_ERROR;
    }
  }
  else{
    /* there is no channel by that name */
    return SS_ERROR;
  }
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

