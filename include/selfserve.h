#ifndef _SELFSERVE_H_
#define _SELFSERVE_H_

#include <malloc.h>
#include "starlocks.h"
#include "queue.h"
#include "customer.h"

void add_coffeepot(void);
void init_selfserve(void);
void pour_coffee(void);

struct coffeepot {
	struct coffeepot *next;
	/* Individual coffeepot lock */
	pthread_mutex_t mutex;
};

struct coffee_station {
	struct coffeepot *pot;
	sem_t available;
	/* TODO: Implement functionality to modify number of available coffeepots */
	pthread_mutex_t mutex;
};

#endif
