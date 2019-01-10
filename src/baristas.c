#/* --------------------------------------------------------------------------*/
/**
 * @baristas.c
 * @author: William Wright
 * 
 * @Synopsis
 *		Multithreaded barista entity 
 */
/* ----------------------------------------------------------------------------*/

#include "../include/baristas.h"
#include "../include/queue.h"
#include "../include/customer.h"


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  complex_barista
 *
 *		complex barista thread entry point  
 *		- handles creating and distributing complex drinks
 *
 * @Param arg
 *
 */
/* ----------------------------------------------------------------------------*/
void *complex_barista (void* arg) 
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
			payment_queue->front = payment_queue->front->next;
		}

		/* Unlock the mutex on the payment queue */
		pthread_mutex_unlock (&payment_queue->mutex);

		/* Was the payment queue empty? */
		if (next_customer == NULL)
			break;

		/* Process payment */
		sim_paytime ();
		/* Clean up */
		free (next_customer);
	}
	return NULL;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  process_payment
 *
 *		Barista cashier thread entry point  
 *		- Process queued jobs (DETACHED threads)
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
			payment_queue->front = payment_queue->front->next;
		}

		/* Unlock the mutex on the payment queue */
		pthread_mutex_unlock (&payment_queue->mutex);

		/* Was the payment queue empty? */
		if (next_customer == NULL)
			break;

		/* Process payment */
		sim_paytime ();
		/* Clean up */
		free (next_customer);
	}
	return NULL;
}
/* --------------------------------------------------------------------------*/
/**
 * @Synopsis process_payment
 *		simulated (timed) payment processing procedure
 */
/* ----------------------------------------------------------------------------*/
void sim_paytime(void) {
}
/* --------------------------------------------------------------------------*/
/**
 * @Synopsis service_c_coffee
 *		barista thread (consumer) handling the complex coffee queue
 *
 * @Param arg
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
void* service_c_coffee (void* arg) 
{
	return NULL;
}

void init_bar(struct barista* b, int wt) 
{
	pthread_mutex_init(&b->mutex, NULL);
	pthread_cond_init(&b->available, NULL);
	b->busy = false;
	b->waittime	= wt;
}

