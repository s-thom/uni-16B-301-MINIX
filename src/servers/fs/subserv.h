/* Hold the types and struct definitions used by the subscription server
 */

#ifndef SUBSERV
#define SUBSERV

#define SS_ERROR 0
#define SS_SUCCESS 1

#define ss_status m3_i1
#define ss_name m3_ca1
#define ss_pointer m3_p1
#define ss_int m3_p1


/* Holds information about a channel */
typedef struct channel CHANNEL;
struct channel {
  char name[15];
  char oid;
  
  long subscribed;
  long unreceived;
  
  int content_size;
  int min_buffer;
  
  void *content;
  struct channel *next;
};

/* Function prototypes */

int do_subserv();
int handle_create();
int handle_close();
int handle_push();
int handle_pull();
int handle_subscribe();
int handle_unsubscribe();
long set_map(int index, int boolean, long current_map);
int get_map(int index, long current_map);

#endif

