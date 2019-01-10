/* --------------------------------------------------------------------------*/
/**
 * @starlocks.c
 * @author	William Wright
 * @date	November 30, 2014
 * @course	cpsc457-fall2014
 * @Synopsis
 *		main controller
 */
/* ----------------------------------------------------------------------------*/
#include "../include/starlocks.h"
/* --------------------------------------------------------------------------*/
/**
 * @Synopsis 
 *		globally defined (thread shared) starlocks lineups / queues
 */
/* ----------------------------------------------------------------------------*/
struct barista*			c_barista;
struct barista*			p_barista;
struct queue*			s_coffee_queue;			/* simple coffee line */
struct queue*			c_coffee_queue;			/* complex coffee line */
struct queue*			payment_queue;
struct coffee_station*	self_serve;		/* self service coffee pots */
struct customer_data*	cdata_dynarr;		/* dynamically allocated thread array */

FILE* complex_log;
FILE* simple_log;

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  
 *		usage: starlocks -t <number of threads>
 *
 *		starlocks is a simulated coffee shop equipped to handle a multithreaded
 *		environment wherein customers are divided into two categories - complex
 *		coffee drinkers (lattes, london fogs, etc) and simple black coffee 
 *		drinkers 
 *
 *		options [bps]	unimplemented
 *
 * @Param argc
 * @Param argv[]
 *		-t				specifies number of customer threads to simulate
 *		-b				specifies number of baristas serving complex coffees
 *		-p				specifies number of baristas handling payments
 *		-s				specifies number of available positions in all queues
 *
 * @Returns
 *		0				on successful completion of simulation
 */
/* ----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	pthread_t *threads;
	char *uopt_arg = NULL;
	int uopt, trial = 0;
	char *usage = "usage: starlocks -t <thread count> [-l <trial>]\n";
	unsigned long uarg;

	opterr = 0;
	while ((uopt = getopt (argc, argv, "t:l:")) != -1) {
		switch (uopt) {
			case 't':
				uopt_arg = optarg;
				break;
			case 'l': 
				trial = atoi(optarg);
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
	
	init_logfiles(uarg, trial);

	/* initialize self serve coffee station */
	init_selfserve(CP_PER_SELFSERVE);
	/* initialize baristas as shared resources */
	init_lineups(uarg);
	/* allocate space on the heap to account for customers, baristas and selfserves */
	init_sr_baristas();
	
	/* initialize lineups / queues */
	threads = (pthread_t *) malloc(sizeof(pthread_t) * uarg + NUM_BARISTAS + NUM_SELFSERVE);
	/* generate customer threads */
	gen_customers(threads, uarg);

	printf("starlocks (main): successfully processed all customer threads, made a killing!\n");
	pthread_exit(NULL);

	fclose (complex_log);
	fclose (simple_log);

	return 0;
}


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis 
 *		initialize lineups/queues - allocate space on heap to account for 
 *		queue data structure - node mem allocation and assignment occurs in
 *		queue.c
 *
 * @Param num_threads
 *		simulate realistic store capacity by specificying a bound on queue size.
 * 
 */
/* ----------------------------------------------------------------------------*/
void init_lineups(unsigned long num_threads) {
	s_coffee_queue	= (struct queue*) malloc(sizeof(struct queue));
	c_coffee_queue	= (struct queue*) malloc(sizeof(struct queue));
	payment_queue	= (struct queue*) malloc(sizeof(struct queue));
	init_queue(s_coffee_queue, num_threads, S_LINE_TITLE);
	init_queue(c_coffee_queue, num_threads, C_LINE_TITLE);
	init_queue(payment_queue, num_threads, P_LINE_TITLE);
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  init_sr_baristas
 *		initialization routine for baristas
 */
/* ----------------------------------------------------------------------------*/
void init_sr_baristas(void) 
{
	p_barista = (struct barista*) malloc(sizeof(struct barista));
	c_barista = (struct barista*) malloc(sizeof(struct barista));
	init_bar(p_barista, PAYMENT_WTIME);
	init_bar(c_barista, COMPLEX_WTIME);
}

void write_to_log (FILE* lfile, const char* data)
{
	fprintf (lfile, "%s\n", data);
}

void init_logfiles (const unsigned long numthreads, int  trialnum) 
{
	char logname[250];

	sprintf(logname, "%s%lu.%d-%s", LOG_DIR, numthreads, trialnum, COMPLEX_LOG);
	complex_log = fopen(logname, "w");
	sprintf(logname, "%s%lu.%d-%s", LOG_DIR, numthreads, trialnum, SIMPLE_LOG);
	simple_log = fopen(logname, "w");

	if(complex_log == NULL || simple_log == NULL) {
		perror("log file initialization");
		exit(1);
	}
}
