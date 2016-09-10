/* Hold the types and struct definitions used by the subscription server
 */
 
 /* Holds information about a channel */
 struct channel {
  char cname[15];
  char cowner;
  
  long subscribed;
  long unrecieved;
  long waiting;
  
  void *content;
  int content_size;
  
  struct channel *next;
 };
