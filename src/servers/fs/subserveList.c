#include "subserv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

WPROC *get_subscriber(int procn, WPROC *root){
  
  while(root != NULL){
    
    if(root->procnr == procn){
      return root;
    }
    root = root->next;
  }
  return NULL;
}

/* moves a WPROC from one list to another */
WPROC *wproc_shift(int procn, WPROC *from, WPROC *to){
  WPROC *toMove = get_subscriber(procn, from);
}

void *destroy_list(WPROC *root){
  WPROC *next = NULL;    

  while(root != NULL){
    next = root->next;
    free(root);
    root = next;       
  }
}

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
        
        if(temp->content != NULL){
          /* frees stored date seg */          
          free(temp->content);
        }
        /* free other link lists */
        destroy_list(temp->unrecieved_list);
        destroy_list(temp->waiting_list);
        destroy_list(temp->recieved_list);

        free(temp);
        return placeHolder;
      }
      else{
        /* its not the root being remove */
        placeHolder = temp;
        prev->next = temp->next;

        /* free other link lists */
        destroy_list(temp->unrecieved_list);
        destroy_list(temp->waiting_list);        

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
  new->next = root;
  return new;
}

/* returns a pointer */
CHANNEL *get_channel(char name[], CHANNEL *root){  
  
  while(root != NULL){    
    if(strncmp(name, root->name, 14) == 0){
      return root;
    }    
    root = root->next;
  }
  return NULL;
}

CHANNEL* create_channel(char *name, char oid, int size) {
  CHANNEL *nc = (CHANNEL*) malloc(sizeof(CHANNEL));
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
  nc->content = NULL;
  nc->content_size = 0;
  nc->min_buffer = size;
  nc->next = NULL;

  nc->unrecieved_list = NULL;
  nc->recieved_list = NULL;
  nc->waiting_list = NULL;
  
  return nc;
}

WPROC *create_waiting(int proc, void *p, int size) {
  WPROC *np = (WPROC*) malloc(sizeof(WPROC));
  
  np->procnr = proc;
  np->content_size = size;
  np->content = p;
  np->next = NULL;
  
  return np;
}

WPROC *create_wproc(int procnr, int content_size, void *content){
  WPROC *node = malloc(sizeof(WPROC));  
  
  node->procnr = procnr;
  node->content_size = content_size;
  node->content = content;
  node->next = NULL;

  return node;
}

WPROC *remove_from_wproc(int procn, WPROC *root){
  WPROC *prev = NULL;
  WPROC *current = root;
  WPROC *result = root;
  
  while(current != NULL){
    
    if(procn == current->procnr){
      if(prev != NULL){
        /* cut out the current node */
        prev->next = current->next;
      }
      else{
        /* return the new root to be the next */
        result = current->next;
      }
      free(current);
      return result;
    }
    prev = current;
    current = current->next; 
  }
  return root;
}

WPROC *wproc_seprate_out(int procn, WPROC *root){
  WPROC *current = root;
  WPROC *prev = NULL;  

  while(current != NULL){
    if(procn == current->procnr){
      if(prev == NULL){
        root = current->next;
      }
      else{
        prev->next = current->next;
      }
    }

    printf("loop v\n");
    prev = current;
    current = current->next;
    
    printf("current proc = %d next = %d\n", current->procnr, current->next->procnr);
    /*
    printf("length of new current: %d\n", wproc_list_length(current));
    printf("loop u\n");
    */
  }
  return root;
}

/* after this call from must be set to NULL!! */
WPROC *list_copy(WPROC *to, WPROC *from){
  
  WPROC *temp = NULL;  
  WPROC *current = to;
  
  /* if the list is null return the from list */
  if(to == NULL){
    printf("[sl] copy to is empty");
    return from;
  }
  /* if the from list is null return the to list */
  if(from == NULL){
    return to;
  }

  while(current->next != NULL){
    current = current->next;
  }

  current->next = from;
  return to;
  
}

int wproc_list_length(WPROC *root){
  int count = 0;
  
  while(root != NULL){
    count++;
    root = root->next;
  }

  return count;
}


