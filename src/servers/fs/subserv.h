/* Hold the types and struct definitions used by the subscription server
 */
 
/* Holds information about a channel */
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
struct channel* create_channel(char *name, char oid);
struct channel* find_channel(char *name);
int handle_create();
int handle_close();
int handle_push();
int handle_pull();
int handle_subscribe();
int handle_unsubscribe();
long set_map(int index, int boolean, long current_map);
int get_map(int index, long current_map);
