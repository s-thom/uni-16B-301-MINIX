/* Types used by subserv */
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

