/* --------------------------------------------------------------------------*/
/**
 * @selfserve.c
 * @author: William Wright
 * 
 * @Synopsis
 *		Handles the majority of the self serve oriented routines 
 */
/* ----------------------------------------------------------------------------*/

#include "../include/selfserve.h"

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis init_selfserve
 *			initialization routine to allocate heap mem for self_serve
 *			counter serving simple coffee customers, in addition to mutex
 *			and conditional initialization amongst other things.
 *
 * @Param num_pots
 *			specifies how many coffee pots are available in the self serve
 */
/* ----------------------------------------------------------------------------*/
void init_selfserve(int num_pots)
{
	int i;

	if(self_serve != NULL || num_pots < 1) 
		exit(1);

	self_serve = (struct coffee_station *) malloc(sizeof(struct coffee_station) );

	if(!self_serve) {
		perror("selfserve (init_selfserve) error");
		exit(1);
	}

	self_serve->num_customers = 0;
	self_serve->pot = NULL;

	pthread_mutex_init(&self_serve->mutex, NULL);
	pthread_cond_init(&self_serve->available, NULL);

	for(i = 0; i < num_pots; i++)
		add_coffeepot(self_serve);
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis 
 *		Must hold mutex lock on selfserve - prior to lock chaining on coffeepot
 *		specific locks.
 *
 *		- lock chaining - left unimplemented for sake of simplicity
 *
 * @Param cust
 *		customer who is currently holding the lock on selfserve
 */
/* ----------------------------------------------------------------------------*/
void* pour_coffee(struct customer* c_cust) 
{
	struct coffeepot* cpot = self_serve->pot;
	unsigned long iter = 0;

	while(cpot != NULL) {
		if(!cpot->occupied) {
			pthread_mutex_lock(&cpot->mutex);
			break;
		} else {
			cpot = cpot->next;
			/* if we've iterated through the cpots
			 * and have not found an available lock
			 * keep iterating */
			if(cpot == NULL)
				cpot = self_serve->pot;

		}
	}

	/* set occupancy flag/boolean for coffeepot */
	cpot->occupied = true;
#if NONAGGRO
	/* remove from entrance queue prior to simulation */
	dequeue_customer(s_coffee_queue);
#endif
	/* lock chaining success - drop the root - prepare for simulation */
	pthread_mutex_unlock(&self_serve->mutex);

	/* simulate coffee pouring - approx. 1 sec on 4 cores */
	for(iter = 0; iter < SIMPLE_WTIME; iter++)
		;
#if NONAGGRO
	enqueue_customer(payment_queue, c_cust);
#endif
	/* decrement number of customers at the self serve */
	self_serve->num_customers = self_serve->num_customers - 1;

	/* reset occupancy flag on coffee pot */
	cpot->occupied = false;
	/* release coffeepot mutex */
	pthread_mutex_unlock(&cpot->mutex);

	/* advise single waiting thread of coffeepot availability */
	pthread_cond_broadcast(&self_serve->available);
	

	return NULL;
}



/* --------------------------------------------------------------------------*/
/**
 * @Synopsis full_selfserve
 *		Used to verify whether the number of customers currently accessing
 *		the self serve station is equal to that of the number of coffee pots
 *		available. 
 *
 * @Returns  
 *			1			-	if the self serve area if at capacity
 *			0			-	otherwise
 */
/* ----------------------------------------------------------------------------*/
int full_selfserve() 
{
	int retval = 0;

	if(self_serve->num_customers == self_serve->num_pots)
		retval = 1;
	else
		retval = 0;

	return retval;

}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  add_coffeepot
 *		Add a coffeepot to a given self serve station
 *
 * @Param station
 *		specifies which station is to be granted an additional coffeepot
 */
/* ----------------------------------------------------------------------------*/
void add_coffeepot(struct coffee_station *station)
{
	struct coffeepot* new_coffeepot;

	pthread_mutex_lock(&station->mutex);

	new_coffeepot = (struct coffeepot*) malloc(sizeof(struct coffeepot));
	
	if(!new_coffeepot) {
		perror ("selfserve (add_coffeepot) error");
		exit (1);
	}

	new_coffeepot->next = self_serve->pot;
	self_serve->pot = new_coffeepot;

	pthread_mutex_init(&new_coffeepot->mutex, NULL);

	new_coffeepot->occupied = false;
	self_serve->num_pots = self_serve->num_pots + 1;

	/* release coffee station (root) lock */
	pthread_mutex_unlock(&station->mutex);
}


