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

CHANNEL *channels = NULL;

PUBLIC int do_subserv() {
  /* TODO: check message status code, act accordingly */
  int retcode;
  char index;
  char terminated = 0;
  
  /* Zero the rest of the name as soon as a \0 is found */
  for (index = 0; index < 15; index++) {
    if (terminated) {
      m_in.ss_name[index] = 0;
    } else {
      if (m_in.ss_name[index] == 0) {
        terminated = 1;
      }
    }
  }
  
  switch(m_in.ss_status) {
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
    default:
      retcode = SS_ERROR;
  }
  
  /* TODO: Send message back */
  m_out.ss_status = retcode;
  
  return OK;
}

/**
 * Handles the creation of a channel
 */
int handle_create() {
  /* Create overview 
   * Check channel doesn't exist
   * Create channel struct, set values
   * Send back
   */
  CHANNEL *chan;
  char ind;
  
  printf("[subserv] got CREATE\n");
  
  chan = get_channel(m_in.m3_ca1);
  
  /* Error checking */
  /* Check channel doesn't exist
   * All other functions make sure the channel *does* exist. Not this one
   */
  if (chan == NULL) {
    /* TODO: Set errno */
    return SS_ERROR;
  }
  
  /* TODO: Write function */
}

/**
 * Handles the closing of a channel
 */
int handle_close() {
  /* Close overview 
   * Check channel exists, sender is owner
   * Remove channel
   * Send back
   */  
  CHANNEL *chan;
  char ind;
  
  printf("[subserv] got CLOSE\n");
  
  chan = get_channel(m_in.m3_ca1);
  
  /* Error checking */
  /* Check channel actually exists */
  if (chan == NULL) {
    /* TODO: Set errno */
    return SS_ERROR;
  }
  /* Check owner of channel */
  if (m_in.m_source != chan->oid) {
    /* TODO: Set errno */
    return SS_ERROR;
  }
  
  channels = remove_channel(m_in.ss_name, channels);
  
  return SS_SUCCESS;
}

/**
 * Handles pushing to a channel
 */
int handle_push() {
  /* Push overview 
   * Check pusher owns channel
   * Free previous stored data
   * Copy data to this
   */
  CHANNEL *chan;
  char ind;
  
  
  printf("[subserv] got PUSH\n");
  
  chan = get_channel(m_in.m3_ca1);
  
  /* Error checking */
  /* Check channel actually exists */
  if (chan == NULL) {
    /* TODO: Set errno */
    return SS_ERROR;
  }
  /* Check owner of channel */
  if (m_in.m_source != chan->oid) {
    /* TODO: Set errno */
    return SS_ERROR;
  }
  
  /* Free previous content, copy new content */
  if (chan->content != NULL) {
    free(chan->content);
  }
  
  if (m_in.ss_int > chan->min_buffer) {
    chan->content_size = chan->min_buffer;
  } else {
    chan->content_size = m_in.ss_int;
  }
  chan->content = malloc(chan->content_size);
  sys_vircopy(m_in.m_source, D, m_in.ss_pointer, SELF, D, chan->content, chan->content_size);
  
  chan->unrecieved = chan->subscribed;
  
  return SS_SUCCESS;
}

/**
 * Handles pulling from a channel
 */
int handle_pull() {
  /* Pull overview 
   * Check puller is subscribed
   * Copy data
   * Set message data
   */
  CHANNEL *chan;
  int copy_size;
  
  printf("[subserv] got PULL\n");
  
  chan = get_channel(&m_in.ss_name, channels);
  
  if (chan == NULL) {
    /* TODO: Set errno */
    return SS_ERROR;
  }
  
  /* Ensure puller is subscribed */
  if (!get_map(m_in.m_source, chan->subscribed)) {
    /* TODO: Set errno */
    return SS_ERROR;
  }
  
  /* Subscribers should only recieve each content once */
  if (!get_map(m_in.m_source, chan->unreceived)) {
    /* TODO: Set errno */
    return SS_ERROR;
  }
  
  /* TODO: Write function */
  /* Find size to copy */
  if (chan->content_size > m_in.ss_int) {
    copy_size = m_in.ss_int;
  } else {
    copy_size = chan->content_size;
  }
  
  /* Copy and set received */
  sys_vircopy(SELF, D, chan->content, m_in.m_source, D, m_in.ss_pointer, copy_size);
  
  chan->unrecieved = set_map(m.in_source, 0, chan->unreceived);
  
  /* Small memory optimisation
   * Free content if there's nothing waiting to receive it
   * Thanks Jayden
   */
  if (chan->unreceived == 0) {
    /* Free previous content, copy new content */
    if (chan->content != NULL) {
      free(chan->content);
      chan->content = NULL;
      chan->content_size = 0;
    }
  }
  
  return SS_SUCCESS;
}

/**
 * Handles subscribing to a channel 
 */
int handle_subscribe() {
  /* Subscribe overview 
   * Check proc isn't already subscribed
   * Add proc to subscribers
   * Send back
   */
  printf("[subserv] got SUBSCRIBE\n");
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles unsubscription from a channel
 */
int handle_unsubscribe() {
  /* Unsubscribe overview 
   * Check proc is subscribed
   * Remove proc from subscribers
   * Send back
   */
  printf("[subserv] got UNSUBSCRIBE\n");
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
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

