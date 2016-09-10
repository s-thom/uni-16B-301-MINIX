/* Hold the types and struct definitions used by the subscription server
 */
 
 /* Holds information about a channel */
 struct channel {
  char[15],
  char owner,
  
  long subscribed,
  long unrecieved,
  long waiting,
  
  void *content,
  int content_size,
  
  struct channel *next
 }
