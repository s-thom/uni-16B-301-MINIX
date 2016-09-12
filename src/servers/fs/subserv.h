/* Hold the types and struct definitions used by the subscription server
 */
 
 /* Holds information about a channel */
 struct channel {
  char name[15];
  char oid;
  
  long subscribed;
  long unreceived;
  long waiting;
  
  void *content;
  int content_size;
  
  struct channel *next;
 };
 
 /* Function prototypes */
 
 int do_subserv();
 struct channel* create_channel(char *name, char oid);
 struct channel* find_channel(char *name);
