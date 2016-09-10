/* Holds the main function for the subscription server
 */
 
#include "fs.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include "param.h"
#include "subserv.h"
#include <stddef.h>
#include <string.h>

struct channel *channels = NULL;

PUBLIC int do_subserv() {
  /* TODO: check message status code, act accordingly */
}

