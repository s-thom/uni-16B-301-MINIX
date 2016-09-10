/* Holds the main function for the subscription server
 */
 
#include "fs.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include "param.h"

#include "subserv.h"
#include <stddef.h>
#include <string.h>

#include <stdio.h>

struct channel *channels = NULL;

PUBLIC int do_subserv() {
  /* TODO: check message status code, act accordingly */
  printf("Message reached server\n");
}

struct channel* find_channel(char *cname) {
  struct channel *curr = channels;
  
  while (curr != NULL) {
    if (strncmp(cname, &curr->cname, 14) == 0) {
      break;
    }
    
    curr = curr->next;
  }
  
  return curr;
}
