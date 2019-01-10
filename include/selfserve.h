#ifndef _SELFSERVE_H_
#define _SELFSERVE_H_

#include <malloc.h>
#include "starlocks.h"
#include "queue.h"
#include "customer.h"

struct coffee_station {
	struct coffeepot	*pot;
	int num_pots;
	int num_customers;
	pthread_cond_t		available;
	pthread_mutex_t		mutex;		
};

struct coffeepot {
	struct coffeepot	*next;		 /* next coffeepot */
	bool				occupied;	 
	pthread_mutex_t		mutex;		 /* lock per pot */	
};


void add_coffeepot(struct coffee_station* station);
void init_selfserve(int num_pots);
int full_selfserve();

#endif
