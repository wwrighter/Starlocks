#include "../include/selfserve.h"

void init_selfserve()
{
	/* selfserve is initially empty */
	self_serve = NULL;

	sem_init (&self_serve->available, 0, 3);
}

/*
void add_coffeepot()
{
	struct coffeepot* new_coffeepot;

	new_coffeepot = (struct coffeepot*) malloc (sizeof (struct coffeepot));
	
	new_coffeepot->next = self_serve->pot;
	self_serve->pot = new_coffeepot;

	sem_post (&self_serve->available);
}
*/
/*
void* pour_coffee(struct customer* cust) 
{
	while(1) {
		struct coffeepot* reserve;

		struct coffee_pot* next_avail_pot;

		sem_wait (&self_serve->available);

		pthread_mutex_lock(&self_serve->mutex);

		reserve = coffee_station;
	}
}
*/
