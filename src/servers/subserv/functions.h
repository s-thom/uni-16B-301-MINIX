/* Function prototypes */

/* main.c */
int main(void);
void get_work(void);

/* subserv.c */
int do_subserv(void);
int handle_create(void);
int handle_close(void);
int handle_push(void);
int handle_pull(void);
int handle_subscribe(void);
int handle_unsubscribe(void);
long set_map(int index, int boolean, long current_map);
int get_map(int index, long current_map);

/* subserveList.c */
int contains_channel(char name[], CHANNEL *root);
CHANNEL *remove_channel(char name[], CHANNEL *root);
CHANNEL *add_channel(CHANNEL *new, CHANNEL *root);
CHANNEL *get_channel(char name[], CHANNEL *root);
CHANNEL* create_channel(char *name, char oid, int size);

