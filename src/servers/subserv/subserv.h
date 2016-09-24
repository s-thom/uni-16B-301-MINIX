#ifndef SUBSERV
#define SUBSERV

#include <minix/ipc.h>
#include <stdlib.h>

#include "type.h"
#include "glo.h"
#include "functions.h"

/* Cheatily define D instead of messily including a lot */
#define D 1

/* Definitions */
#define SS_ERROR 0
#define SS_SUCCESS 1

#define ss_status m3_i1
#define ss_name m3_ca1
#define ss_pointer m3_p1
#define ss_int m3_i2

#endif

