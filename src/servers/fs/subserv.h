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


/* Holds information about a channel */
typedef struct channel CHANNEL;
struct channel {
  char name[15];
  char oid;
  
  int content_size;
  int min_buffer;
  
  void *content;
  struct channel *next;
  
  struct wproc *waiting_list;
  struct wproc *unrecieved_list;
  struct wproc *recieved_list;
};

/* A process that is waiting for a message */
/* Originally called this because it represented a [W]aiting [PROC]ess that is blocked unitl the channel's owner pushes.
 * The name has stuck since.
 */
typedef struct wproc WPROC;
struct wproc {
  int procnr;
  int content_size;
  
  void *content;
  struct wproc *next;
};

/* Function prototypes */
/* subserv.c */
int do_subserv(void);
int handle_create(void);
int handle_close(void);
int handle_push(void);
int handle_pull(void);
int handle_subscribe(void);
int handle_unsubscribe(void);
int handle_info(void);
int copy_to_proc(int proc, void *pointer, int size, CHANNEL *chan);
/* subservList.c */
CHANNEL *create_channel(char *name, char oid, int size);
CHANNEL *add_channel(CHANNEL *new, CHANNEL *root);
CHANNEL *remove_channel(char name[], CHANNEL *root);
CHANNEL *get_channel(char name[], CHANNEL *root);
int contains_channel(char name[], CHANNEL *root);
WPROC *create_wproc(int procnr, int content_size, void *content);
WPROC *remove_from_wproc(int procn, WPROC *root);
WPROC *wproc_seprate_out(int procn, WPROC *root);
WPROC *wproc_shift(int procn, WPROC *from, WPROC *to);
WPROC *list_copy(WPROC *to, WPROC *from);
WPROC *get_subscriber(int procn, WPROC *root);
int wproc_list_length(WPROC *root);
void *destroy_list(WPROC *root);

#endif

