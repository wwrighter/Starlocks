/* --------------------------------------------------------------------------*/
/**
 * @customer.c
 * @author: William Wright
 * 
 * @Synopsis
 *		Multithreaded customer entity 
 */
/* ----------------------------------------------------------------------------*/
#include "../include/customer.h"

/* Key used to associate data specific to each thread */
static pthread_key_t cdata_key;

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis cust_entrance
 *
 *	entry function for customer threads, handles thread-specific data 
 *	initialization and progression / functionality of customer tasks
 *
 * @Param data (void *)
 *		heap allocated pointer to customer data struct
 *
 * @Returns t_runtime (double)  
 *		turnaround time experienced by customer (thread runtime)
 */
/* ----------------------------------------------------------------------------*/
void* cust_entrance(void *data) 
{
	pthread_attr_t attr;
	size_t t_stacksize;
	double t_runtime;

	struct customer_data *t_data = (struct customer_data *) data;	
	pthread_attr_getstacksize (&attr, &t_stacksize);

	/* set thread-specific data */
	pthread_setspecific (cdata_key, t_data);

	struct customer* new_node = (struct customer*) malloc(sizeof(struct customer));
	new_node->info = t_data;

	/* wait for service, order/selfserve and pay */
	get_coffee(new_node);

	/* Return to starlocks.c (main) with value of total running time for this customer */
	gettimeofday(&t_data->end_time, NULL);
	t_runtime = ( (t_data->end_time.tv_sec - t_data->start_time.tv_sec)  + ( (t_data->end_time.tv_usec - t_data->start_time.tv_usec) / 1000000.0 ) );
	pthread_exit((void*) &t_runtime);
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis get_coffee
 *		customer service routine :)  handles thread safe concurrent execution
 *		via a few subroutines - where in safe concurrent access is employed.
 *
 * @Param s_cust
 *		customer to whom would like some coffee
 * @Returns
 *		0			-	on success
 */	
/* ----------------------------------------------------------------------------*/
int get_coffee(struct customer* s_cust)
{
	int ret = 0;

#if NONAGGRO
	/* step into type specific line (complex/simple) */
	struct queue* entrance_line = ( (s_cust->info->type == 1) ? s_coffee_queue : c_coffee_queue);

	/* step into line */
	enqueue_customer(entrance_line, s_cust);
#endif

	/* Branch based on customer type - simple or complex */
	switch(s_cust->info->type) {
		case 1:
			pthread_mutex_lock(&self_serve->mutex);

#if NONAGGRO
			/* while customer is not front of the line and the selfserve is full */
			while (full_selfserve() || s_cust->info->f_pos != true) 
				pthread_cond_wait(&self_serve->available, &self_serve->mutex);
#else
			while (full_selfserve())
				pthread_cond_wait(&self_serve->available, &self_serve->mutex);
#endif
			self_serve->num_customers = self_serve->num_customers + 1;

			/* coffee pouring simulation - see selfserve */
			pour_coffee(s_cust);
			break;
		case 2:
			pthread_mutex_lock(&c_barista->mutex);

#if NONAGGRO
			/* 
			 * ensure once we obtain signal that we are in fact the front
			 * of the line and the barista is not busy.
			 */
			while (c_barista->busy || c_coffee_queue->front != s_cust) 
				pthread_cond_wait(&c_barista->available, &c_barista->mutex);
#else
			while (c_barista->busy)
				pthread_cond_wait(&c_barista->available, &c_barista->mutex);
#endif
			/* barista serving complex drinks is available, proceed */
			get_served(s_cust);
			break;
		default:
			printf("customer (get_coffee) : default case encountered.  rogue customer!\n");
			return -1;
		}

		checkout(s_cust);
		return ret;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  get_served
 *			used in conjunction to complex coffee drink processing
 *			- ensures proper flags on the barista are set, and handles
 *			transitioning from one line to the next (payment)
 *
 * @Param s_cust
 */
/* ----------------------------------------------------------------------------*/
void get_served(struct customer* s_cust) 
{
	c_barista->busy = true;
	wait_coffee();
#if NONAGGRO
	dequeue_customer(c_coffee_queue);
	enqueue_customer(payment_queue, s_cust);
#endif
	c_barista->busy = false;
	pthread_mutex_unlock(&c_barista->mutex);			
	pthread_cond_broadcast(&c_barista->available);
}
/* --------------------------------------------------------------------------*/
/**
 * @Synopsis gen_customers
 *
 *	handles creation and default pthread initialization/setup of all customer 
 *	threads - including dynamic allocation at runtime to meet command line thread
 *	requests and thread stack space optimization.
 *
 * @Param num_customers
 *
 *		specified at runtime (command line arg) specifying number of threads
 *		to be executed as customer entities.
 */
/* ----------------------------------------------------------------------------*/
void gen_customers(pthread_t* threads, unsigned int num_customers)
{
	unsigned int i, c_count, s_count, rnum = 0;
	int nt;
	pthread_attr_t attr;
	size_t d_stacksize;
	double* running_time;
	char rtime_str[80];
	double avgturn_simple = 0.0;
	double avgturn_complex = 0.0;


	/* Dynamically allocate data for new customer on heap */
	cdata_dynarr = (struct customer_data*) malloc(sizeof(struct customer_data) * num_customers);

	pthread_attr_init(&attr);
	/* Ensure we can account for all customers to avoid early exit */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	/* Generate stack size based off configuration settings - reduce overhead */
	d_stacksize = sizeof(struct customer_data);

	/* Create associative key for thread-specific data */
	pthread_key_create (&cdata_key, (void* ) &c_destructor);

	pthread_attr_setstacksize (&attr, d_stacksize);
	for(i = 0; i < num_customers; i++) {
		cdata_dynarr[i].num = i;
		/* Generate customer type (complex/simple) random */
		srand(i + time(NULL));
		rnum = (rand() % 2) + 1;
		cdata_dynarr[i].type = rnum;
		/* Initialize the start_time to symbolize entrance into Starlocks */
		gettimeofday(&cdata_dynarr[i].start_time, NULL);
		nt = pthread_create(&threads[i], &attr, (void* ) &cust_entrance, (void *) &cdata_dynarr[i]);
		if(nt) {
			printf("error: return code from pthread_create() is %d\n", nt);
			exit(-1);
		}
	}

	/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);

	s_count = 0;
	c_count = 0;

	for (i = 0; i < num_customers; i++) {
		nt = pthread_join(threads[i], (void**) &running_time);
		if(nt) {
			printf("error: return code from pthread_join() is %d\n", nt);
			exit(-1);
		}
		switch(cdata_dynarr[i].type) 
		{
			case 1:
				++s_count;
				avgturn_simple += (double) *running_time;
				snprintf(rtime_str, 80, "%d\t%f\t%d", s_count, (double) *running_time, i+1);
				write_to_log(simple_log, rtime_str);
				break;
			case 2:
				++c_count;
				avgturn_complex += (double) *running_time;
				snprintf(rtime_str, 80, "%d\t%f\t%d", c_count, (double) *running_time, i+1);
				write_to_log(complex_log, rtime_str);
				break;
			default:
				fprintf(stderr, "error: unable to determine log file for data.\n");
				break;
		}
	}

	snprintf(rtime_str, 80, "\n------\n# of simple customers: %d", s_count);
	write_to_log(simple_log, rtime_str);
	snprintf(rtime_str, 80, "\n------\n# of complex customers: %d", c_count);
	write_to_log(complex_log, rtime_str);

	snprintf(rtime_str, 80, "accumulated time: %f seconds", avgturn_simple);
	write_to_log(simple_log, rtime_str);
	snprintf(rtime_str, 80, "accumulated time: %f seconds", avgturn_complex);
	write_to_log(complex_log, rtime_str);

	snprintf(rtime_str, 80, "avg turnaround: %f seconds", avgturn_simple / s_count);
	write_to_log(simple_log, rtime_str);
	snprintf(rtime_str, 80, "avg turnaround: %f seconds", avgturn_complex / c_count);
	write_to_log(complex_log, rtime_str);


}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis c_destructor
 *
 *		customer thread "destructor" - clean up routine.
 *
 * @Param cdata
 *		pointer to customer entity pthread associated key
 */
/* ----------------------------------------------------------------------------*/
void c_destructor (void* cdata)
{
#ifdef CUSTOMER_DESTRUCTOR
	struct customer_data* d_customer = (struct customer_data*) cdata;
	printf("c_destructor: customer id: %d\n", d_customer->num);
	// TODO: Further implementation
#endif
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis wait_coffee
 *		juvenile wait time simulation based on COMPLEX_WTIME 
 *		 - defaulted (provided) value is approximately 2.0 seconds
 */
/* ----------------------------------------------------------------------------*/
void wait_coffee(void)
{
	unsigned long i = 0;
	for(i = 0; i < COMPLEX_WTIME; i++)
		;;
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis checkout
 *		proceed through shared resource (payment barista) and exit 
 *		out of the store (recording elapsed time - before killing oneself.
 */
/* ----------------------------------------------------------------------------*/
void checkout(struct customer* cust) 
{
	/* Alright, we are currently in the queue for the payment */
	pthread_mutex_lock(&p_barista->mutex);
#if NONAGGRO	
	while (payment_queue->front != cust || p_barista->busy) 
		pthread_cond_wait(&p_barista->available, &p_barista->mutex);
#else
	while (p_barista->busy)
		pthread_cond_wait(&p_barista->available, &p_barista->mutex);
#endif
	p_barista->busy = true;

	time_killer(cust);
	
	p_barista->busy = false;
#if NONAGGRO
	dequeue_customer(payment_queue);
#endif
	pthread_mutex_unlock(&p_barista->mutex);

	pthread_cond_broadcast(&p_barista->available);
}

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis  time_killer
 *			title says it all
 *
 * @Param cust
 */
/* ----------------------------------------------------------------------------*/
void time_killer(struct customer* cust)
{
	unsigned long i = 0;

	for(i = 0; i < 5000000; i++)
		;
}
