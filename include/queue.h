#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>
#include "starlocks.h"

extern struct queue* s_coffee_queue;
extern struct queue* c_coffee_queue;
extern struct queue* payment_queue;

struct customer {
	struct customer *next;
	struct customer_data *info;
};

struct queue {
 	struct customer *front;
 	struct customer *end;
	unsigned long size;
	int num_customers;
	/* mutex lock for safe concurrent access to queue */
	pthread_mutex_t mutex;
	/* pthread condition used in signal and entry list */
	pthread_cond_t queue_occupied;
	pthread_cond_t queue_not_full;
};

void init_queue(struct queue *q, unsigned long q_size);
unsigned int customer_count(struct queue *q);
bool empty_queue(struct queue *q);
bool full_queue(struct queue *q);
bool enqueue_customer(struct queue* line, struct customer* new_customer);
struct customer* dequeue_customer(struct queue* line);
void clear_queue(struct queue *q);
struct customer* dequeue_customer(struct queue* line);
#endif
