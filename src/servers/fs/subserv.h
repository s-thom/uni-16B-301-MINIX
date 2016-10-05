/* Hold the types and struct definitions used by the subscription server
 */

#ifndef SUBSERV
#define SUBSERV

#define SS_ERROR 0
#define SS_SUCCESS 1

#define ss_status m3_i1
#define ss_name m3_ca1
#define ss_pointer m3_p1
#define ss_int m3_i2

typedef struct double_int DOUBLE_INT;
struct double_int{
  long lower;
  long upper;
};

/* Holds information about a channel */
typedef struct channel CHANNEL;
struct channel {
  char name[15];
  char oid;
  
  DOUBLE_INT subscribed;
  DOUBLE_INT unreceived;
  DOUBLE_INT waiting;
  
  int content_size;
  int min_buffer;
  
  void *content;
  struct channel *next;
  struct wproc *waiting_list;
};

/* A process that is waiting for a message */
typedef struct wproc WPROC;
struct wproc {
  int procnr;
  int content_size;
  
  void *content;
  struct wproc *next;
};

/* Function prototypes */

int do_subserv(void);
int handle_create(void);
int handle_close(void);
int handle_push(void);
int handle_pull(void);
int handle_subscribe(void);
int handle_unsubscribe(void);
int handle_info(void);
DOUBLE_INT set_map(int index, int boolean, DOUBLE_INT current_map);
int get_map(int index, DOUBLE_INT current_map);
int copy_to_proc(int proc, void *pointer, int size, CHANNEL *chan);
int bitsSetInLong(DOUBLE_INT l);

#endif

