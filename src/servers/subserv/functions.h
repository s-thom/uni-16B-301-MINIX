/* Function prototypes */

/* main.c */
int main();
void get_work();

/* subserv.c */
int do_subserv();
int handle_create();
int handle_close();
int handle_push();
int handle_pull();
int handle_subscribe();
int handle_unsubscribe();
long set_map(int index, int boolean, long current_map);
int get_map(int index, long current_map);

/* subserveList.c */
int contains_channel(char name[], CHANNEL *root);
CHANNEL *remove_channel(char name[], CHANNEL *root);
CHANNEL *add_channel(CHANNEL *new, CHANNEL *root);
CHANNEL *get_channel(char name[], CHANNEL *root);
CHANNEL* create_channel(char *name, char oid, int size);

