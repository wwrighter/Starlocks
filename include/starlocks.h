#ifndef _STARLOCKS_H_
#define _STARLOCKS_H_

#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/time.h>

#include "../include/customer.h"

#define NUM_BARISTAS		2
#define NUM_SELFSERVES		1

#define DEBUG_CUSTOMER		1
#define DEBUG_QUEUE			1
#define DEBUG_BARISTAS		1
#define DEBUG_SELFSERVE		1

void init_lineups(unsigned long q_size);
void init_selfserve ();
void gen_customers (pthread_t*, unsigned long);

#endif
