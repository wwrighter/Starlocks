#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>
#include <string.h>
#include "starlocks.h"

#define MAX_TITLE		50

extern struct queue* s_coffee_queue;
extern struct queue* c_coffee_queue;
extern struct queue* payment_queue;

struct customer {
	struct customer *next;
	struct customer_data *info;
};

struct queue {
 	struct customer*	front;
 	struct customer*	end;
	unsigned long		size;
	unsigned int		num_customers;
	char				title[MAX_TITLE];
	/* mutex lock for safe concurrent access to queue */
	pthread_mutex_t		mutex;
	/* pthread condition used in signal and entry list */
	pthread_cond_t		queue_occupied;
	pthread_cond_t		queue_not_full;
};

void init_queue(struct queue *q, unsigned long q_size, char* q_title);
unsigned int customer_count(struct queue *q);
bool empty_queue(struct queue *q);
bool full_queue(struct queue *q);
bool enqueue_customer(struct queue* line, struct customer* new_customer);
struct customer* dequeue_customer(struct queue* line);
void clear_queue(struct queue *q);
struct customer* dequeue_customer(struct queue* line);
#endif
