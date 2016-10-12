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
#include <unistd.h>

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
      retcode = handle_info();
      break;
    default:
      m_out.ss_status = SS_ERROR;
      retcode = OK;
  }
  
  return retcode;
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
  CHANNEL *chan = NULL;
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
  
  /* now we create the channel */
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
  WPROC *temp_proc;
  WPROC *waiting_proc;
  message *proc_reply;
  char ind;
  CHANNEL *chan = get_channel(m_in.m3_ca1, channels);
  
  /* Error checking */
  /* Check channel actually exists */
  if (chan == NULL) {
    m_out.ss_status = SS_ERROR;
    return OK;
  }
  /* Check owner of channel */
  if (m_in.m_source != chan->oid) {
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
  /* chan->unreceived = chan->subscribed; */
  
  /* shift everything in recieved to unrecieved */
  chan->unrecieved_list = list_copy(chan->unrecieved_list, chan->recieved_list);
  chan->recieved_list = NULL;
  /* now that received is empty point it to null */

  /* Clear all waiting procs */
  while (chan->waiting_list != NULL) {
    waiting_proc = chan->waiting_list;


    /* Reply to process (unblocking) */
    proc_reply = (message*) malloc(sizeof(message));
    
    /* Copy over */
    proc_reply->ss_int = copy_to_proc(waiting_proc->procnr, waiting_proc->content, waiting_proc->content_size, chan);
    
    proc_reply->ss_status = SS_SUCCESS;
    _send(waiting_proc->procnr, proc_reply);
    free(proc_reply);    
    
    chan->waiting_list = waiting_proc->next; /* removes the the first elem off waitingList */
    waiting_proc->next = NULL; /* unnec but testing */
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
    m_out.ss_status = SS_ERROR;
    return OK;
  }

  /* Ensure puller is subscribed */
  if (!(get_subscriber(sender, chan->unrecieved_list) || get_subscriber(sender, chan->recieved_list) || get_subscriber(sender, chan->waiting_list))) {
    m_out.ss_status = SS_ERROR;
    return OK;
  }
  
  /* Subscribers should only recieve each content once */
  if (chan->content == NULL || get_subscriber(sender, chan->recieved_list)) {
    /* move out of reviece and into waiting */
    toShift = get_subscriber(sender, chan->recieved_list);
    
    if(toShift == NULL){
      toShift = get_subscriber(sender, chan->unrecieved_list);
      chan->recieved_list = wproc_seprate_out(sender, chan->unrecieved_list);
    }
    else{
      chan->recieved_list = wproc_seprate_out(sender, chan->recieved_list);        
    }
    
    toShift->content = m_in.ss_pointer;
    toShift->content_size = m_in.ss_int;

    toShift->next = chan->waiting_list;
    chan->waiting_list = toShift; 

    return SUSPEND;
  }
  
  m_out.ss_int = copy_to_proc(m_in.m_source, m_in.ss_pointer, m_in.ss_int, chan);
  m_out.ss_status = SS_SUCCESS;

  return OK;
}

WPROC *create_wproc(int procnr, int content_size, void *content){
  WPROC *node = malloc(sizeof(WPROC));  
  
  node->procnr = procnr;
  node->content_size = content_size;
  node->content = content;
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
  WPROC *new;

  chan = get_channel(m_in.ss_name, channels);
  sender = m_in.m_source;

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

    sleep(5);

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
  /* m_in */

  chan = get_channel(m_in.ss_name, channels);
  sender = m_in.m_source;  
  
  /* checks to see if that channel is in the list */
  if(chan != NULL){
    
    if(get_subscriber(sender, chan->unrecieved_list) || get_subscriber(sender, chan->waiting_list)){
     /* if they are in either list then remove them from the list */
     chan->unrecieved_list = remove_from_wproc(sender, chan->unrecieved_list);
     chan->waiting_list = remove_from_wproc(sender, chan->waiting_list);
     chan->recieved_list = remove_from_wproc(sender, chan->recieved_list);
 
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

  int waiting = 0;
  int unreceived = 0;
  int received = 0;  
  
  if(currentC == NULL){
  }
  else{
    printf("--------------------------------------------------------------------------------\n");
    while(currentC != NULL){
      
      waiting = wproc_list_length(currentC->waiting_list);
      unreceived = wproc_list_length(currentC->unrecieved_list);
      received = wproc_list_length(currentC->recieved_list);

      printf("%s : %d/%d\n", currentC->name, waiting + unreceived, waiting + unreceived + received);
      currentC = currentC->next;
    }
    
    printf("--------------------------------------------------------------------------------\n");
  }
  m_out.ss_status = SS_SUCCESS;
  return OK;
}

int copy_to_proc(int proc, void *pointer, int size, CHANNEL *chan) {
  int copy_size;

  WPROC *temp = NULL;  

  /* Find size to copy */
  if (chan->content_size > size) {
    copy_size = size;
  } else {
    copy_size = chan->content_size;
  }
  
  /* Copy and set received */
  sys_vircopy(SELF, D, chan->content, proc, D, pointer, copy_size);
  /* move from whatever list to recieved list */
  temp = get_subscriber(proc, chan->waiting_list);
  if(temp == NULL){
    temp = get_subscriber(proc, chan->unrecieved_list);
    chan->unrecieved_list = wproc_seprate_out(proc, chan->unrecieved_list);
  }
  else{
    chan->waiting_list = wproc_seprate_out(proc, chan->waiting_list);
  }

  if(temp == NULL){
    printf("copy to proc failed due to not being able to find target");
    return 0;
  }

  temp->next = chan->recieved_list;
  chan->recieved_list = temp;

  return copy_size;
}

