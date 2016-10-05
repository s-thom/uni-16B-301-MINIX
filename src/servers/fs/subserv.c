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
    /* INFO */
    case 6:
      retcode = info();
      break;
    default:
      m_out.ss_status = SS_ERROR;
      retcode = OK;
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
  chan = get_channel(m_in.m3_ca1, channels);
  
  /* Error checking */
  /* Check channel doesn't exist
   * All other functions make sure the channel *does* exist. Not this one
   */
  if (chan != NULL) {
    /* Check owner of channel */
    if (m_in.m_source == chan->oid) {
      /* You own this, yay! */
      m_out.ss_status = SS_SUCCESS;
      return OK;
    }
  
    /* TODO: Set errno */
    m_out.ss_status = SS_ERROR;
    return OK;
  }
  
  chan = create_channel(m_in.ss_name, m_in.m_source, m_in.ss_int);
  channels = add_channel(chan, channels);
  
  m_out.ss_status = SS_SUCCESS;
  return OK;
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
  chan = get_channel(m_in.m3_ca1, channels);
  
  /* Error checking */
  /* Check channel actually exists */
  if (chan == NULL) {
    /* TODO: Set errno */
    m_out.ss_status = SS_ERROR;
    return OK;
  }
  /* Check owner of channel */
  if (m_in.m_source != chan->oid) {
    /* TODO: Set errno */
    m_out.ss_status = SS_ERROR;
    return OK;
  }
  
  channels = remove_channel(m_in.ss_name, channels);
  
  m_out.ss_status = SS_SUCCESS;
  return OK;
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
  WPROC *waiting_proc;
  WPROC *unrecieved_proc;
  WPROC *temp_proc;
  message *proc_reply;
  char ind;
  chan = get_channel(m_in.m3_ca1, channels);
  /* set the unrecieved_proc to the unrecieved List */
  unrecieved_proc = chan->unrecieved_list;
  
  /* Error checking */
  /* Check channel actually exists */
  if (chan == NULL) {
    /* TODO: Set errno */
    m_out.ss_status = SS_ERROR;
    return OK;
  }
  /* Check owner of channel */
  if (m_in.m_source != chan->oid) {
    /* TODO: Set errno */
    m_out.ss_status = SS_ERROR;
    return OK;
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
  chan->unreceived = chan->subscribed;
  
  /* Clear all waiting procs */
  while (chan->waiting_list != NULL) {
    waiting_proc = chan->waiting_list;
    
    /* Copy over */
    copy_to_proc(waiting_proc->procnr, waiting_proc->content, waiting_proc->content_size, chan);
    
    /* Reply to process (unblocking) */
    proc_reply = (message*) malloc(sizeof(message));
    proc_reply->ss_status = SS_SUCCESS;
    _send(waiting_proc->procnr, proc_reply);
    free(proc_reply);    

    /* Move to unrecived */
    temp_proc = waiting_proc->next;
    waiting_proc->next = unrecieved_proc;
    unrecieved_proc = waiting_proc;
    waiting_proc = temp_proc;
  }
  
  m_out.ss_status = SS_SUCCESS;
  return OK;
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
  int sender = m_in.m_source;
  WPROC *toShift = NULL;  

  chan = get_channel(m_in.ss_name, channels);
  
  if (chan == NULL) {
    /* TODO: Set errno */
    m_out.ss_status = SS_ERROR;
    return OK;
  }
  
  /* Ensure puller is subscribed */
  if (!get_map(m_in.m_source, chan->subscribed)) {
    /* TODO: Set errno */
    m_out.ss_status = SS_ERROR;
    return OK;
  }
  
  /* Subscribers should only recieve each content once */
  if (chan->content == NULL || !get_map(m_in.m_source, chan->unreceived)) {
    /* move out of reviece and into waiting */
    toShift = get_subscriber(, from);
    
    return SUSPEND;
  }
  
  copy_to_proc(m_in.m_source, m_in.ss_pointer, m_in.ss_int, chan);
  
  m_out.ss_status = SS_SUCCESS;
  return OK;
}

WPROC *create_wproc(int procnr, int content_size, void *content){
  WPROC *node = malloc(sizeof(WPROC));  
  
  node->procnr = procnr;
  node->content_size = content_size;
  node->content = *content;
  node->next = NULL;

  return node;
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
  CHANNEL *chan;
  int sender;
  /* TODO: Check for erroneous message */
  /* TODO: Write function */

  temp = get_channel(m_in.ss_name, channels);
  sender = m_in.m_source;
  WPROC new;

  /* checks to insure the channel exists */
  if(chan != NULL){
    /* m_out.ss_int = temp->min_buffer */
    /* checks to see if already subscribed */
    if(get_subscriber(sender, chan->unrecieved_list) || get_subscriber(sender, chan->waiting_list)){
      /* the channel is already subscribed */
      m_out.ss_status = SS_SUCCESS;
      return OK;
    }
    
    /* add to the unrecieved list */
    new = create_wproc(sender, m_in.ss_int, m_in.ss_pointer);
    new->next = chan->unrecieved_list;
    chan->unrecieved_list = new;

    m_out.ss_int = chan->min_buffer;
    m_out.ss_status = SS_SUCCESS; 
    return OK;

  }
  else{
    /* the channel doesnt exist that you tried to subscribe to */
    m_out.ss_status = SS_ERROR;
    return OK;
  }     
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
  CHANNEL *chan;
  int sender;
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
  /* m_in */

  chan = get_channel(m_in.ss_name, channels);
  sender = m_in.m_source;  
  
  /* checks to see if that channel is in the list */
  if(chan != NULL){
    
    if(get_subscriber(sender, chan->unrecieved_list) || get_subscriber(sender, chan->waiting_list)){
     /* if they are in either list then remove them from the list */
     chan->unrecieved_list = remove_from_wproc(sender, chan->unrecieved_list);
     chan->waiting_list = remove_from_wproc(sender, chan->waiting_list);
      
     m_out.ss_status = SS_SUCCESS;
     return OK;
    }
    else{
      /* its trying to unsubscribe from channel its not subed 2 */
      m_out.ss_status = SS_ERROR;
      return OK;
    }
  }
  else{
    /* there is no channel by that name */
    m_out.ss_status = SS_ERROR;
    return OK;
  }
}


int handle_info(void){
  CHANNEL *currentC = channels;  
  
  if(currentC == NULL){
    printf("There are currently no channels :(");
  }
  else{
    printf("--------------------------------------------------------------------------------\n");
    while(currentC != NULL){
      
      printf("%s : %d/%d\n", currentC->name, bitsSetInLong(currentC->unreceived), bitsSetInLong(currentC->subscribed));
      currentC = currentC->next;
    }
    
    printf("--------------------------------------------------------------------------------\n");
  }
  m_out.ss_status = SS_SUCCESS;
  return OK;
}

/* checks how many bits are set in a bit map */
int bitsSetInLong(long l){
  int count = 0;
  int i = 0;
  long mask = 0x01;

  printf("long is %ld", l);

  while(i < 64){
  
    if((l & mask) == 1){
      count++;
    }
    l = l>>1;  
    i++;
  }
  return count;
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

int copy_to_proc(int proc, void *pointer, int size, CHANNEL *chan) {
  int copy_size;
  /* Find size to copy */
  if (chan->content_size > size) {
    copy_size = size;
  } else {
    copy_size = chan->content_size;
  }
  
  /* Copy and set received */
  sys_vircopy(SELF, D, chan->content, proc, D, pointer, copy_size);

  chan->unreceived = set_map(proc, 0, chan->unreceived);
  
  return 1;
}

