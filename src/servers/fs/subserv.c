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
  nc->name[14] = 0;
  
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

/**
 * Handles the creation of a channel
 */
int handle_create() {
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles the closing of a channel
 */
int handle_close() {
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles pushing to a channel
 */
int handle_push() {
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles pulling from a channel
 */
int handle_pull() {
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles subscribing to a channel
 */
int handle_subscribe() {
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}

/**
 * Handles unsubscription from a channel
 */
int handle_unsubscribe() {
  /* TODO: Check for erroneous message */
  /* TODO: Write function */
}





