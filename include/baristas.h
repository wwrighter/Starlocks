#ifndef _BARISTAS_H_
#define _BARISTAS_H_

#include "starlocks.h"
#include "queue.h"
#include "customer.h"

struct barista {
	pthread_mutex_t		mutex;
	pthread_cond_t		available;
	int					waittime;
	bool				busy;
};

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis 
 *		external functions / variables
 */
/* ----------------------------------------------------------------------------*/
extern struct queue*	payment_queue;
extern struct queue*	s_coffee_queue;
extern struct queue*	c_coffee_queue;
extern struct barista*	p_barista;
extern struct barista*  c_barista;
/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  
 *		local function prototypes
 */
/* ----------------------------------------------------------------------------*/
void* process_payment (void* arg);
void sim_paytime(void);
void init_bar(struct barista* b, int wt);
void* service_c_coffee(void* arg);

#endif
