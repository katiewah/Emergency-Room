#include <stdio.h>
#include <stdlib.h>
#include "sim.h"


/////////////////////////////////////////////////////////////////////////////////////////////
//
// General Purpose Discrete Event Simulation Engine
//
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Simulation Engine Data Structures
/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data structure for an event; note this is designed to be independent of the application domain.
// Each event can have parameters defined within the application. We want the simulation engine
// not to have to know the number or types of these parameters, since they are dependent on the
// application, and we want To keep the engine independent of the application. To address this problem
// each event only contains a single parameter, a pointer to the application defined parameters (AppData).
// The simulation engine only knows it has a pointer to the event parameters, but does not know the
// structure (number of parameters or their type) of the information pointed to.
// This way the event can have application-defined information, but the simulation engine need not
// know the number or type of the application-defined parameters.
//
struct Event {
	double timestamp;		// event timestamp
    void* AppData;			// pointer to application defined event parameters
	struct Event *Next;		// priority queue pointer
	int custType;           //
	int nextComp;
};

// Simulation clock variable
double Now = 0.0;

// Future Event List
// Use an event structure as the header for the future event list (priority queue)
// Using an event struct as the header for the priority queue simplifies the code for
// inserting/removing events by eliminating the need to explicitly code special cases
// such as inserting into an empty priority queue, or removing the last event in the priority queue.
// See the Remove() and Schedule() functions below.
struct Event FEL ={-1.0, NULL, NULL};

/////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes for functions used within the Simulation Engine
/////////////////////////////////////////////////////////////////////////////////////////////

// Function to print timestamps of events in event list
void PrintList (void);

// Function to remove smallest timestamped event
struct Event* Remove (void);

/////////////////////////////////////////////////////////////////////////////////////////////
// Simulation Engine Functions Internal to this module
/////////////////////////////////////////////////////////////////////////////////////////////

// Remove smallest timestamped event from FEL, return pointer to this event
// return NULL if FEL is empty
struct Event* Remove (void)
{
    struct Event* e;
    
    if (FEL.Next==NULL) return (NULL);
    e = FEL.Next;		// remove first event in list
    FEL.Next = e->Next;
    return (e);
}

// Print timestamps of all events in the event list (used for debugging)
void PrintList (void)
{
    struct Event *p;
    
    /*printf ("\n\nEvent List: ");
    for (p=FEL.Next; p!=NULL; p=p->Next) {
        printf ("%f ", p->timestamp);
    }
    printf ("\n");*/
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Simulation Engine functions visible to simulation application
/////////////////////////////////////////////////////////////////////////////////////////////
//
// Note the strategy used for dynamic memory allocation. The simulation engine is responsible
// for freeing any memory it allocates, and is ONLY responsible for freeing memory allocated
// within the simulation engine. Here, the simulation dynamically allocates memory
// for each event put into the event list. The Schedule function allocates this memory.
// This memory is released after the event is processed (in RunSim), i.e., after the event handler
// for the event has been called and completes execution.
// Because we know each event is scheduled exactly once, and is processed exactly once, we know that
// memory dynamically allocated (using malloc) for each event will be released exactly once (using free).
// Similarly, the simulation application (not shown here) is responsible for reclaiming all memory
// it dynamically allocates, but does not release any memory allocated by the simulation engine.
//

// Return current simulation time
double CurrentTime (void)
{
	return (Now);
}

// Schedule new event in FEL
// queue is implemented as a timestamp ordered linear list

void Schedule (double ts, void *data, int custType, int nextComp)
{
	struct Event *e, *p, *q;

	// create event data structure and fill it in
	if ((e = malloc (sizeof (struct Event))) == NULL) exit(1);
	e->timestamp = ts;
	e->AppData = data;
	e -> custType = custType;
	e -> nextComp = nextComp;
	printf("typeReg: %d\n",e -> custType);

	// insert into priority queue
	// p is lead pointer, q is trailer
	for (q=&FEL, p=FEL.Next; p!=NULL; p=p->Next, q=q->Next) {
		if (p->timestamp >= e->timestamp) break;
		}
	// insert after q (before p)
	e->Next = q->Next;
	q->Next = e;
}

double PrioritySchedule (double ts, void *data, int custType, int nextComp)
{	
	int flag = 0;
	struct Event *e, *p, *q;
	double end;
	double endFlag = 0;
	double add;

	// create event data structure and fill it in
	if ((e = malloc (sizeof (struct Event))) == NULL) exit(1);
	e->timestamp = ts;
	e->AppData = data;
	e -> custType = custType;
	e -> nextComp = nextComp;
	printf("typePQ: %d\n",e -> custType);

	// insert into priority queue
	// p is lead pointer, q is trailer
	for (q=&FEL, p=FEL.Next; p!=NULL; p=p->Next, q=q->Next) {
		if (p -> nextComp >= 6) {
			printf("\n%f   ",p -> timestamp);
			printf("%d   ",p -> custType);
			printf("%d   ",p -> nextComp);
		}		
	}

	printf("\n\n");

	for (q=&FEL, p=FEL.Next; p!=NULL; p=p->Next, q=q->Next) {
		printf("here");
		if ((p->custType < e->custType) && (p->nextComp == e->nextComp)) break;
		printf("%d   %d,      %d   %d\n", p->custType, e->custType, p->nextComp, e->nextComp);
		if (p -> Next == NULL) {
			flag = 1;
		}
	}
	
	// insert after q (before p)
	e->Next = q->Next;
	q->Next = e;

	if (flag == 0) {
		e -> timestamp = p -> timestamp;

	} else {
		if (e -> custType == 1) {
			add = 15;
		} else if (e -> custType == 2) {
			//printf("here1\n");
			add = 30;
		} else if (e -> custType == 3) {
			add =  60;
		} else if (e -> custType == 4) {
			add = 90;
		}
		e -> timestamp = q -> timestamp + add;
		end = e -> timestamp + ts;
		endFlag = 1;	
	}

	//returnsTime(e -> timestamp);
	printf("here2");	
	
	//changes timestamps of future events that need to be changed
	//updates sTime for the component appropriatelty
	for (q= q -> Next, p=p; p!=NULL; p=p->Next, q=q->Next) {
		if ((p->custType < e->custType) && (p->nextComp >= e->nextComp))
			p -> timestamp = p -> timestamp + ts; 
			if (p->nextComp == e->nextComp) {
				if ((p -> custType == 1) && (endFlag == 0)) {
					end = p -> timestamp + 15;
				} else if ((p -> custType == 2) && (endFlag == 0)) {
					end = p -> timestamp + 30;
				} else if ((p -> custType == 3) && (endFlag == 0)) {
					end = p -> timestamp + 60;
				} else if ((p -> custType == 4) && (endFlag == 0)) {
					end = p -> timestamp + 90;
				}
			}
			
			//end = 
	}

	for (q=&FEL, p=FEL.Next; p!=NULL; p=p->Next, q=q->Next) {
		if (p -> nextComp >= 6) {
			printf("\n%f   ",p -> timestamp);
			printf("%d   ",p -> custType);
			printf("%d   ",p -> nextComp);
		}
	}
		
	printf("\n\n");

	return(end); 
}



// double returnsTime(double ts) {
// 	return ts;
// }

// Function to execute simulation up to a specified time (EndTime)
void RunSim (double EndTime)
{
	double done = end(EndTime);
	struct Event *e;

	printf ("\nInitial event list:\n");
	PrintList ();

	// Main scheduler loop
	while ((e=Remove()) != NULL) {
		Now = e->timestamp;
        if (Now > EndTime) {
        	printf("Simulation Finished");
        	break;
        }
		EventHandler(e->AppData, done);
		free (e);	// it is up to the event handler to free memory for parameters
        PrintList ();
	}
	if ((e=Remove()) == NULL) {
		printf("end");
	}
}

double end (double EndTime) {
	return EndTime;
}

