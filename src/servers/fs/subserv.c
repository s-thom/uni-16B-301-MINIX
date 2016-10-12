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
  int retcode;
  char index;
  char terminated = 0;
  
  m_out.ss_int = 0;
  
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
  
  /* OK sends a reply back to process
   * SUSPEND doesn't (and blocks it until a message is sent) 
   * Must ensure that m_out.ss_status is set
   */
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
  message *proc_reply;
  chan = get_channel(m_in.m3_ca1, channels);
  
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
  
  /* Unblock all waiting processes */
  proc_reply = (message*) malloc(sizeof(message));
  while (chan->waiting_list != NULL) {
    /* Nothing was copied, errors happened */
    proc_reply->ss_int = 0;
    proc_reply->ss_status = SS_ERROR;
    _send(chan->waiting_list->procnr, proc_reply);
    chan->waiting_list = chan->waiting_list->next; 
  }
  free(proc_reply);  
  
  /* goodbye */
  channels = remove_channel(m_in.ss_name, channels);
  
  m_out.ss_status = SS_SUCCESS;
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
  /* And I'm FREEEEEEEEEEEEEE */
  if (chan->content != NULL) {
    /* FREEEEFAAAALLLINNN' */
    free(chan->content);
  }
  
  /* Only copy over the minimum of the actual content size and what the channel owner said they'd send when creating the channel */
  if (m_in.ss_int > chan->min_buffer) {
    chan->content_size = chan->min_buffer;
  } else {
    chan->content_size = m_in.ss_int;
  }
  /* Allocate and copy */
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

    chan->waiting_list = waiting_proc->next; /* removes the the first elem off waitingList */

    /* Reply to process (unblocking) */
    proc_reply = (message*) malloc(sizeof(message));
    /* Copy over, set ss_int field to the size copied */
    proc_reply->ss_int = copy_to_proc(waiting_proc, chan);
    /* Set mesage status, send message (unblocks reciever), free message */
    proc_reply->ss_status = SS_SUCCESS;
    _send(waiting_proc->procnr, proc_reply);
    free(proc_reply);    
    
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
  WPROC *proc;

  chan = get_channel(m_in.ss_name, channels);

  if (chan == NULL) {
    m_out.ss_status = SS_ERROR;
    return OK;
  }

  /* Ensure puller is subscribed */
  if (!(proc = get_subscriber(sender, chan->unrecieved_list))) {
    if(!(proc = get_subscriber(sender, chan->recieved_list))) {
      if(!(proc = get_subscriber(sender, chan->waiting_list))) {
        /* Puller isn't in any of the lists */
        m_out.ss_status = SS_ERROR;
        return OK;
      }
    }
  }
  
  proc->content = m_in.ss_pointer;
  proc->content_size = m_in.ss_int;
  
  /* Subscribers should only recieve each content once */
  /* If this statement is true, puller should be blocked */
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

    toShift->next = chan->waiting_list;
    chan->waiting_list = toShift;

    return SUSPEND;
  }
  
  /* Copy data to puller, send back size copied */
  m_out.ss_int = copy_to_proc(proc, chan);
  m_out.ss_status = SS_SUCCESS;

  return OK;
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

    /* Send back the size the channel owner indicated should be allocated */
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
  /* Prints info for all channels */
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

/* Helper function to do the virtual copy from subserv to another process */
int copy_to_proc(WPROC *proc, CHANNEL *chan) {
  int copy_size;  

  /* Find size to copy */
  if (chan->content_size > proc->content_size) {
    copy_size = proc->content_size;
  } else {
    copy_size = chan->content_size;
  }
  
  /* Copy and set received */
  sys_vircopy(SELF, D, chan->content, proc->procnr, D, proc->content, copy_size);
  /* move from whatever list to recieved list */
  if(get_subscriber(proc->procnr, chan->unrecieved_list)){
    chan->unrecieved_list = wproc_seprate_out(proc->procnr, chan->unrecieved_list);
  } else {
    chan->waiting_list = wproc_seprate_out(proc->procnr, chan->waiting_list);
  }

  /* Move process into the recieved list */
  proc->next = chan->recieved_list;
  chan->recieved_list = proc;

  return copy_size;
}

