#include "subserv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* checks if the channel is already in the list and if it is return 1 */
int contains_channel(char name[], CHANNEL *root){
  
  while(root != NULL){
    if(strncmp(name, root->name, 14) == 0){
      return 1;
    }
    root = root->next;
  }
  return 0;
}

/* removes string at a given index */
CHANNEL *remove_channel(char name[], CHANNEL *root){
  
  int current = 0;
  CHANNEL *temp = root;
  CHANNEL *prev = NULL;
  CHANNEL *placeHolder = NULL;
  
  while(temp != NULL){
    if(strncmp(name, temp->name, 14) == 0){
      if(prev == NULL){
        /* its the root */
        placeHolder = temp->next;
        free(temp);
        return placeHolder;
      }
      else{
        /* its not the root being remove */
        placeHolder = temp;
        prev->next = temp->next;
        free(temp);
        return root;
      }
    }
    prev = temp;
    temp = temp->next;    
    current++;
  }
  return NULL;
}

/* adds the new node to the front of the list */
CHANNEL *add_channel(CHANNEL *new, CHANNEL *root){
  printf("adding struct %s\n", new->name);  
  new->next = root;
  return new;
}

/* returns a pointer */
CHANNEL *get_channel(char name[], CHANNEL *root){  
  
  while(root != NULL){
    printf("comp = %s and %s\n", name, root->name);
    if(strncmp(name, root->name, 14) == 0){
      return root;
    }    
    root = root->next;
  }
  return NULL;
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
