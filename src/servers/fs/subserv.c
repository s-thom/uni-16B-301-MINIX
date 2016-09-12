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
}





