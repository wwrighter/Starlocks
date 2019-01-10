#/* --------------------------------------------------------------------------*/
/**
 * @queue.c
 * @author: William Wright
 * 
 * @Synopsis
 *		Queue (lineup) functionality with thread safe concurrent access
 */
/* ----------------------------------------------------------------------------*/
#include "../include/queue.h"
#include "../include/starlocks.h"
#include "../include/customer.h"
/* --------------------------------------------------------------------------*/
/**
 * @Synopsis enqueue_customer
 *
 *		insert specified customer "node" into the specified queue utilizing safe
 *		concurrency via mutex lock - if the queue is full on entrance, 
 *		relinguish obtained lock until signal is received (queue_occupied)
 *
 * @Param line
 *		queue of interest to dequeue (remove one customer from from of queue)
 * @Param new_customer
 *		new customer entity to be inserted into queue
 * @Returns   
 *		true	- on success
 *		false	- otherwise
 */
/* ----------------------------------------------------------------------------*/
bool enqueue_customer (struct queue* line, struct customer* new_customer)
{
		pthread_mutex_lock(&line->mutex);
	
		if(full_queue(line))
			pthread_cond_wait (&line->queue_not_full, &line->mutex);

		if(line->end == NULL) 
			line->front = new_customer;
		else 
			line->end->next = new_customer;

		/* add customer to end of queue */
		line->end = new_customer;

		/* increase the customer count for queue by 1 */
		line->num_customers = line->num_customers + 1;

		/* relinquish lock */
		pthread_mutex_unlock (&line->mutex);

		/* 
		 * signal to those threads interested that the queue
		 * is not full, though it is occupied.
		 */
		pthread_cond_signal (&line->queue_occupied);

#ifdef DEBUG_QUEUE
		printf("successfully enqueued thread.\n");
#endif

		return true;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis dequeue_customer
 *
 *		remove a customer "node" from the specified queue utilizing safe
 *		concurrency via mutex lock - if the queue is empty on entrance, 
 *		relinguish obtained lock until signal is received.
 *
 * @Param line
 *		queue of interest to dequeue (remove one customer from from of queue)
 * @Param old_customer
 *
 * @Returns   
 *		pointer to dequeued customer entity, NULL if invalid.
 */
/* ----------------------------------------------------------------------------*/
struct customer* dequeue_customer (struct queue* line) 
{
	struct customer* d_customer;
		
		/* pthread_mutex_lock will not return until mutex is available */
		pthread_mutex_lock (&line->mutex);

		d_customer = line->front;
		line->front = d_customer->next;

		/* release lock and block, waiting for new customers */
		if(empty_queue(line)) { 
			line->end = NULL;
			pthread_cond_wait(&line->queue_occupied, &line->mutex);
		}

		if(d_customer != NULL)
			line->num_customers = line->num_customers - 1;

		pthread_mutex_lock (&line->mutex);
		pthread_cond_signal (&line->queue_not_full);

	return d_customer;
}


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis init_queue
 *		initialization of specified queue, size is in the queue structure
 *		which currently adds a slight amount of overhead given it is not 
 *		currently used.
 *
 * @Param q
 *		queue to initialize
 * @Param q_size
 *		size of queue
*/
/* ----------------------------------------------------------------------------*/
void init_queue(struct queue *q, unsigned long q_size) 
{
//	sem_init (&q->num_customers, 0, 0);
	q->num_customers = 0;
	q->size = q_size;
	q->front = NULL;
	q->end = NULL;
	pthread_cond_init(&q->queue_not_full, NULL);
	pthread_cond_init(&q->queue_occupied, NULL);
	pthread_mutex_init(&q->mutex, NULL);
}


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis empty_queue
 *		expects: mutex lock specific to respective queue is currently held
 *
 * @Param q
 *		queue of interest
 *
 * @Returns   
 *		true  - if queue is empty
 *		false - otherwise
 */
/* ----------------------------------------------------------------------------*/
bool empty_queue(struct queue *q)
{
	if(q->num_customers <= 0)
		return 1;
	else
		return 0;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis full_queue
 *		expects: mutex lock specific to respective queue is currently held
 *
 * @Param q
 *		queue of interest
 *
 * @Returns   
 *		true  - if queue is full
 *		false - otherwise
 */
/* ----------------------------------------------------------------------------*/
bool full_queue(struct queue *q) 
{
	if(q->num_customers == q->size)
		return 1;
	else
		return 0;
}
