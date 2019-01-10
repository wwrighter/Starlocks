#include "../include/baristas.h"
#include "../include/queue.h"
#include "../include/customer.h"


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  process_payment
 *
 * Process queued jobs until the queue is empty.
 *
 * @Param arg
 *
 */
/* ----------------------------------------------------------------------------*/
void *process_payment (void* arg) 
{
	while(1) {
		struct customer *next_customer;

		/* Lock the mutex on the payment queue */
		pthread_mutex_trylock (&payment_queue->mutex);
		/* Obtained lock, check for existence of customer in queue */
		if(payment_queue == NULL)
			/* Avoid escaping with mutex (permanent lock) */
			next_customer = NULL;
		else {
			/* Get the next available customer */
			next_customer = payment_queue->front;
			/* Remove this customer from the payment queue */
//			payment_queue->front = payment_queue->front->next;
		}

		/* Unlock the mutex on the payment queue */
		pthread_mutex_unlock (&payment_queue->mutex);

		/* Was the payment queue empty? */
		if (next_customer == NULL)
			break;

		/* Process payment */
//		process_payment (next_customer);
		/* Clean up */
		free (next_customer);
	}
	return NULL;
}

void *service_c_coffee (void* arg) 
{
}
