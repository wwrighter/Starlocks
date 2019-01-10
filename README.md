# Starlocks
Starlocks Coffee; C; pthreads; shared resources; queues; gnuplot; timing; mutex locks

This repository is meant to serve as a simulation of both scenarios
from the context below. Additionally, we look at how we might be able
to time the scenarios included and plot such timings using gnuplot.

I tried to comment the code as best I could to help both myself and 
others interested in learning easily identify how the functions, 
pthreads, mutex locks, shared resources, and other functionality 
included works. 

The self-service "coffee pots" are shared resources and need to be
protected against unsynchronized access.  

The queues are also shared resources (baristas are servicing customers
and customers are entering the queue). In other words, there is an
ordered sequence of customer threads, not a disordered set of
customers clustering around each barista (except when we simulate "MACFO"
(Most Aggressive Customer First Order). Customers should receive FCFS 
service in their respective queues.

There are two types of jobs: simple and complex. Simple jobs take half
as long as complex jobs (one way to do this is to supply each customer
with a number that the barista or coffee pot must count to in order to
"generate" their coffee; the customer supplies this number to the
barista or coffee pot).

# Context: 

Starlocks Coffee, Inc. is considering a switch in their style of
serving coffee.  Their current model is a single line of customers
being serviced by two baristas.  Some customers have complex demands
while others want a simple cup of black coffee.  Customers arrive in
an unpredictable mix but remain in the same queue in FIFO order.  As
they reach the front of the queue, customers receive service at the
first available barista.  Service consists of ordering, waiting for
the barista to process the job, and paying.  Each of these three
activities occurs with the same barista.  Unfortunately, there are
many more addicted customers than baristas, and sometimes
customers leave the end of the line without paying. Starlocks wants 
to avoid losing this
business.

An executive has a brainwave one day and proposes that customers with
simple demands can access an array of coffee choices on their own.
This self-service model dedicates one barista to "complex" jobs; it
dedicates the other barista to completing the payment process for all
customers.  Customers with complex jobs queue at the first barista.
Customers with simple jobs queue at the three available "self-service"
coffee pots (one queue, three pots).  When finished getting a cup of
coffee, self-service customers queue to wait for service from the
second "payment" barista.  When finished ordering and after receiving
their complex job from the first barista, "complex" customers also
queue at the second "payment" barista (i.e., there is a single payment
queue in FIFO order).

# Assumptions: 

Both types of customers are drawn from a uniform random distribution; 
the simulation should be able to handle a generic customer thread whose 
preferences are chosen at runtime (i.e. thread creation).

In the "new" model, there are always three "self-service" coffee
choices, and they are always "full" (they are never depleted).  A
subset of the three may be locked / in use by existing customers, of
course.
