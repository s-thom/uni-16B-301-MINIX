#include <string.h>
#include <stdio.h>

#include "subserv.h"
#include "subserv.c"

int main() {
  /* TODO: Any init work */
  
  while (1) {
    get_work();
    
    do_subserv();
  }
}

void get_work() {
  /* Unsure if more needs to be done here, but if more does, then it can go here */
  receive(ANY, &m_in);
}
