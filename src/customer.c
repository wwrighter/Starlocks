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

#ifdef CUSTOMER_DEBUG
	printf("cust_entrance: num : %d , type: %d , start_time: %f\n", t_data->num, t_data->type, (double) (t_data->start_time.tv_sec + (t_data->start_time.tv_usec / 1000000.0)) ); 
	printf("cust_entrance: end_time : %f\n", t_data->end_time.tv_usec / 1000000.0);
	printf("cust_entrance: thread %d : stack size = %li bytes \n", t_data->num, t_stacksize);
#endif

	pthread_setspecific (cdata_key, t_data);

#ifdef CUSTOMER_DEBUG
	display_customer();
#endif

	/* step into line */
	struct queue* entrance_line = (t_data->type ? s_coffee_queue : c_coffee_queue);
	struct customer* new_node = (struct customer*) malloc(sizeof(struct customer));
	new_node->info = t_data;

	enqueue_customer(entrance_line, new_node);

	/* Until next time.... */
	gettimeofday(&t_data->end_time, NULL);
	t_runtime = ( (t_data->end_time.tv_sec - t_data->start_time.tv_sec)  + ( (t_data->end_time.tv_usec - t_data->start_time.tv_usec) / 1000000.0 ) );
	pthread_exit((void*) &t_runtime);
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
void gen_customers(pthread_t* threads, unsigned long num_customers)
{
	int i, rnum = 0;
	int nt;
	pthread_attr_t attr;

	// TODO: Remove
	//	void *status;

	size_t d_stacksize;
	double* running_time;

	/* Dynamically allocate data for new customer on heap */
	customer_data_dynarr = (struct customer_data*) malloc(sizeof(struct customer_data) * num_customers);

	pthread_attr_init(&attr);
	// TODO: Remove
	/* Ensure we can account for all customers to avoid early exit */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	/* Generate stack size based off configuration settings - reduce overhead */
	d_stacksize = sizeof(struct customer_data) * 1000 * 1000 + 1000000;

	/* Create associative key for thread-specific data */
	pthread_key_create (&cdata_key, (void* ) &clear_cdata);

	pthread_attr_setstacksize (&attr, d_stacksize);
	for(i = 0; i < num_customers; i++) {

#ifdef DEBUG_CUSTOMER	
		printf("gen_customers: creating thread %d\n", i);
#endif

		customer_data_dynarr[i].num = i;
		/* Generate customer type (complex/simple) random */
		srand(i + time(NULL));
		rnum = (rand() % 2) + 1;
		customer_data_dynarr[i].type = rnum;
		/* Initialize the start_time to symbolize entrance into Starlocks */
		gettimeofday(&customer_data_dynarr[i].start_time, NULL);
		nt = pthread_create(&threads[i], &attr, (void* ) &cust_entrance, (void *) &customer_data_dynarr[i]);
		if(nt) {
			printf("error: return code from pthread_create() is %d\n", nt);
			exit(-1);
		}
	}

	/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for (i = 0; i < num_customers; i++) {
		nt = pthread_join(threads[i], (void**) &running_time);
		if(nt) {
			printf("error: return code from pthread_join() is %d\n", nt);
			exit(-1);
		}

#ifdef DEBUG_CUSTOMER		
//		printf("gen_customers: completed join with thread %d with status of %ld\n", i, (long) status);
		printf("total running time of thread %d was: %f seconds.\n", i, *running_time);
#endif

	}
}


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis display_customer
 *
 *		debugging and general performance testing for customer entity routine
 */
/* ----------------------------------------------------------------------------*/
void display_customer(void) 
{
#ifdef DEBUG_CUSTOMER
	struct customer_data *t_data = (struct customer_data *) pthread_getspecific (cdata_key);
	printf("display_customer: customer number = %d , customer type = %d\n", t_data->num, t_data->type);
#endif
}


/* --------------------------------------------------------------------------*/
/**
 * @Synopsis clear_data
 *
 *		customer thread "destructor" - clean up routine.
 *
 * @Param cdata
 *		pointer to customer entity pthread associated key
 */
/* ----------------------------------------------------------------------------*/
void clear_cdata (void* cdata)
{
	// TODO: Further implementation
}
