#ifndef _BARISTAS_H_
#define _BARISTAS_H_

#include "starlocks.h"
#include "queue.h"
#include "customer.h"

extern struct queue* payment_queue;
extern struct queue* s_coffee_queue;
extern struct queue* c_coffee_queue;

void *process_payment (void* arg);

#endif
