#ifndef _CUSTOMER_H_
#define _CUSTOMER_H_

#include <sys/time.h>
#include "starlocks.h"
#include "queue.h"


#define STACK_SIZE	39.1		/* MB */

extern struct customer_data* customer_data_dynarr;
extern struct coffee_station* self_serve;
extern struct queue* c_coffee_queue;
extern struct queue* s_coffee_queue;
extern struct queue* payment_queue;

//extern bool enqueue_customer(struct queue* line, struct customer* new_customer);

void clear_cdata(void* cdata);
void* cust_entrance(void* data);
void display_customer(void);

struct customer_data {
	int num;
	unsigned short int type;
	struct timeval start_time;
	struct timeval end_time;
};

#endif
