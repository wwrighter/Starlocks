#ifndef _STARLOCKS_H_
#define _STARLOCKS_H_

#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/time.h>

#include "customer.h"
#include "baristas.h"

/* Uncomment for queue oriented FIFO customers */
//#define NONAGGRO (1)

/* Logging related */
#define LOG_DIR				"log/"
#define COMPLEX_LOG			"complex.dat"
#define SIMPLE_LOG			"simple.dat"

/* Line / Queue titles */
#define S_LINE_TITLE		"simple"
#define C_LINE_TITLE		"complex"
#define P_LINE_TITLE		"payment"

/* Approximate wait times */
#define PAYMENT_WTIME		5000000	
#define COMPLEX_WTIME		10000000	
#define SIMPLE_WTIME		5000000		

/* Service setup */
#define NUM_BARISTAS		2		
#define CP_PER_SELFSERVE	3		/* coffee pots per self serve */
#define NUM_SELFSERVE		1

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  
 *		locally defined function prototypes
 */
/* ----------------------------------------------------------------------------*/
void init_lineups(unsigned long q_size);
void init_sr_baristas(void);
void init_logfiles(const unsigned long numthreads, int trialnum);
void write_to_log(FILE* lfile, const char* data);
#endif
