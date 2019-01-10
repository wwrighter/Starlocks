#include "../include/starlocks.h"

struct queue* s_coffee_queue;	/* simple coffee line */
struct queue* c_coffee_queue;	/* complex coffee line */
struct queue* payment_queue;

struct coffee_station* self_serve;	/* self service coffee pots */

/* Used in customer data initialization - permits dynamic thread number specification */
struct customer_data* customer_data_dynarr;

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  
 *
 * @Param argc
 * @Param argv[]
 *
 * @Returns   
 */
/* ----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	pthread_t *threads;
	char *uopt_arg = NULL;
	int uopt, i, rnum;
	char *usage = "usage: starlocks -t <thread count> \n";
	unsigned long uarg;

	opterr = 0;
	while ((uopt = getopt (argc, argv, "t:")) != -1) {
		switch (uopt) {
			case 't':
				uopt_arg = optarg;
				break;
			case '?':
			default:
				fprintf(stderr, usage);
				return(-1);
		}
	}

	/*
	 * Handle the case when the user does not enter any arguments
	 * associated to the t option.
	 */
	if(opterr || uopt_arg == NULL) {
		fprintf(stderr, usage);
		return(-1);
	}

	uarg = strtoul(uopt_arg, NULL, 10);
	if(!uarg || uarg < 1) {
		fprintf(stderr, usage);
		return(-1);
	}

	init_lineups(uarg);
	threads = (pthread_t *) malloc(sizeof(pthread_t) * uarg + NUM_BARISTAS + NUM_SELFSERVES);
	gen_customers(threads, uarg);

	printf("starlocks: main thread - program completed. exiting.\n");
	pthread_exit(NULL);

	return 0;
}

void init_lineups(unsigned long num_threads) {
	s_coffee_queue	= (struct queue*) malloc(sizeof(struct queue));
	c_coffee_queue	= (struct queue*) malloc(sizeof(struct queue));
	payment_queue	= (struct queue*) malloc(sizeof(struct queue));
	init_queue(s_coffee_queue, num_threads);
	init_queue(c_coffee_queue, num_threads);
	init_queue(payment_queue, num_threads);
}

