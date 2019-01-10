#ifndef _CUSTOMER_H_
#define _CUSTOMER_H_

#include <sys/time.h>
#include "starlocks.h"
#include "queue.h"
#include "selfserve.h"
#include "baristas.h"

#define STACK_SIZE	39.1		/* MB */

struct customer_data {
	unsigned int num;
	unsigned short int type;
	bool f_pos;
	struct timeval start_time;
	struct timeval end_time;
};

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis 
 *		external functions / global vars
 */
/* --------------------------------------------------------------------------*/
extern struct customer_data*	cdata_dynarr;
extern struct coffee_station*	self_serve;
extern struct queue*			c_coffee_queue;
extern struct queue*			s_coffee_queue;
extern struct queue*			payment_queue;
extern struct barista*			p_barista;
extern struct barista*			c_barista;

extern FILE* complex_log;
extern FILE* simple_log;

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  
 *		locally defined function prototypes
 */
/* ----------------------------------------------------------------------------*/
void c_destructor(void* cdata);
void* cust_entrance(void* data);
void display_customer(void);
void gen_customers (pthread_t*, unsigned int);
int get_coffee(struct customer* s_cust);
void get_served(struct customer* s_cust);
/* --------------------------------------------------------------------------*/
/**
 * @Synopsis 
 *		customer specific data structure(s)
 */ 
/* ----------------------------------------------------------------------------*/

void* pour_coffee(struct customer* cust);
void wait_coffee(void);
void checkout(struct customer* cust);
void time_killer(struct customer* cust);
#endif
