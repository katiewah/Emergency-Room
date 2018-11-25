#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "sim.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Queueing Network Simulation derived from Sample Gas Station Simulation
//
/////////////////////////////////////////////////////////////////////////////////////////////

int P1Count = 1;
int P2Count = 1;
int P3Count = 1;
int P4Count = 1;

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data structures for event data
//
/////////////////////////////////////////////////////////////////////////////////////////////

// EVENT TYPES:
//  GENERATE: Generation of a new customer
//      Parameters: Component where customer is generated
//  ARRIVAL: Customer arrives at a queue station, fork, or exit component
//      Parameters: Pointer to arriving customer and component where customer arrives
//  DEPARTURE: a customer departs from a queue station
//      Parameters: ...

// Event types: Generate new customer, arrival at a component, departure from queue station
#define     GENERATE1    1
#define     GENERATE2    2
#define     GENERATE3    3
#define     GENERATE4    4
#define     ARRIVAL      5
#define     DEPARTURE    6

// A single data structure is used to handle all three event types
struct EventData {
    int EventType;              // Type of event (GENERATE, ARRIVAL, DEPARTURE)
    struct Customer* Cust;      // Arriving or departing customer; unused for GENERATE events
    int CompID;                 // ID of component where customer created, arrives, or departs
};

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Structures for Customers and FIFO queue
//
/////////////////////////////////////////////////////////////////////////////////////////////

// Customers
struct Customer {
    int type;                   // type of patient based on injury severity
    double CreationTime;        // time customer was created
    double ExitTime;            // time customer departs system
    struct Customer *next;      // pointer to next customer when it is queued in a FIFO
    double WaitTime;            //waiting time of customer
};

// Linked List of customers (FIFO Queue)
struct FIFOQueue {
    struct Customer* first;     // pointer to first customer in queue
    struct Customer* last;      // pointer to last customer in queue
    struct Customer* temp;    
};

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Structures for Components
//
/////////////////////////////////////////////////////////////////////////////////////////////

// maximum number of components that can be defined for the simulation
#define MAXCOMPONENTS    100

// Component types
#define     GENERATOR       0
#define     FORK            1
#define     QUEUE_STATION   2
#define     EXIT            3
//
// Generator Component
//
struct Generator {
    double IntArrTime;     // mean interarrival time for generated components
    int DestComp;          // ID of next component customers are sent to
    int Count;             // number of items that are entering the system
};


//
// Exit component
//
struct Exit {
    int Count;                // number of customers that exited at this component
    double maxSysTime;        //max time customer is in system             
    double minSysTime;        //min time customer is in system   
    double totalSysTime;      //total time customer is in system       
    double maxCustWait;       //max time customer is waiting throughout system
    double minCustWait;       //min time customer is waiting throughout system 
    double totalCustWait;     //total time all customers are waiting        
};

//
// Fork component
//
struct Fork {
    int distribution;     // number of output fields 
    double* probArr;      // probabilities that customer goes to field i 
    int* fieldArr;        // ID of next component customers are sent to
};

//
// Station component
//
struct Station {
    double avgServiceTime;         //service time
    int DestComp;                  // ID of next component customers are sent to
    struct FIFOQueue* fifo;        //fifo queue at every station
    double sTime;                  //scheduled time of the last scheduled event
    double wait;                   //average wait time of a customer in that station
    int count;                     //number of customers who go though that station
};

//
// global, master list of components
//
struct {
    int ComponentType;             // GENERATOR, QUEUE_STATION, FORK, EXIT
    void *Comp;                    // Pointer to information on component (Generator, Exit struct, etc.)
} Component[MAXCOMPONENTS];


/////////////////////////////////////////////////////////////////////////////////////////////
//
//  GLOBLE VARIABLES for analytics
//
/////////////////////////////////////////////////////////////////////////////////////////////
int compCount = 0;                  //number of components created

//
// Functions to create new components
//

// Create a generator component with identifier GenID and interarrival time IntTime
// that is connected to component DestID

///////////////////////////////////
// Generator for type 1 Patients
///////////////////////////////////
struct Generator MakeGenerator1 (int GenID, double IntTime, int DestID)
{   struct Generator* p;
    struct EventData* d;
    struct EventData* d2;
    compCount++;
    
    // Add component to master list; Caller is responsible for handling set up errors
    Component[GenID].ComponentType = GENERATOR;
    printf ("Creating Generator Component, ID=%d, Interarrival time=%f, Destination=%d\n", GenID, IntTime, DestID);
    
    // Allocate space for component, fill in parameters
    if ((p = malloc (sizeof(struct Generator))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    p->IntArrTime = IntTime;
    p->DestComp = DestID;
    p-> Count = 0;
    Component[GenID].Comp = p;
    
    // schedule initial, first generator event for this component for patient of type 1
    if ((d = malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = GENERATE1;
    d->Cust = NULL;
    //d -> Cust -> type = 1;
    d->CompID = GenID;
    printf("%f\n", CurrentTime());
    Schedule (IntTime, d);  // should use random draw here

    return *p;
}

struct Generator MakeGenerator2 (int GenID, double IntTime, int DestID)
{   struct Generator* p2;
    struct EventData* d;
    struct EventData* d2;
    compCount++;
    
    // Add component to master list; Caller is responsible for handling set up errors
    Component[GenID].ComponentType = GENERATOR;
    printf ("Creating Generator Component, ID=%d, Interarrival time=%f, Destination=%d\n", GenID, IntTime, DestID);
    
    // Allocate space for component, fill in parameters
    if ((p2 = malloc (sizeof(struct Generator))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    p2->IntArrTime = IntTime;
    p2->DestComp = DestID;
    p2-> Count = 0;
    Component[GenID].Comp = p2;
    
    // schedule initial, first generator event for this component for patient of type 1
    if ((d2 = malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d2->EventType = GENERATE2;
    d2->Cust = NULL;
    //d -> Cust -> type = 1;
    d2->CompID = GenID;
    printf("%f\n", CurrentTime());
    Schedule (IntTime, d2);  // should use random draw here

    return *p2;
}


// Create an Exit Component with identifier ExitID
void MakeExit (int ExitID)
{   struct Exit *p;
    
    printf ("Creating Exit Component, ID=%d\n", ExitID);
    Component[ExitID].ComponentType = EXIT;
    
    // Allocate space for component, fill in parameters
    if ((p=malloc (sizeof(struct Exit))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    p->Count = 0;
    Component[ExitID].Comp = p;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Function prototypes
//
/////////////////////////////////////////////////////////////////////////////////////////////

// prototypes for event handlers
void Generate1 (struct EventData *e, int IAT);    // generate new customer
void Generate2 (struct EventData *e, int IAT); 
void Arrival (struct EventData *e);     // arrival at a component

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Event Handlers
// Parameter is a pointer to the data portion of the event
//
/////////////////////////////////////////////////////////////////////////////////////////////
//
// Note the strategy used for dynamic memory allocation. The simulation application is responsible
// for freeing any memory it allocates, and is ONLY responsible for freeing memory allocated
// within the simulation application (i.e., it is NOT responsioble for freeing memory allocated
// within the simulation engine). Here, the simulation dynamically allocates memory
// for the PARAMETERS of each event. Whenever a new event is scheduled, memory for the event's paramters
// is allocated using malloc. This storage is released within the event handler when it is done
// processing the event.
// Because we know each event is scheduled exactly once, and is processed exactly once, we know that
// memory dynamically allocated for each event'a parameters will be released exactly once.
// Note that within the simulation engine memory is also dynamically allocated and released. That memory
// is different from the memory allocated here, and the simulation application is not concerned with
// memory allocated in the simulation engine.
//

// General Event Handler Procedure defined in simulation engine interface called by simulation engine
void EventHandler (void *data)
{
    struct EventData *d;
    
    // coerce type so the compiler knows the type of information pointed to by the parameter data.
    d = (struct EventData *) data;
    // call an event handler based on the type of event
    if (d->EventType == GENERATE1) Generate1 (d, 10);
    else if (d->EventType == GENERATE2) Generate2 (d, 18);
    else if (d->EventType == ARRIVAL) Arrival (d);
    else {
        fprintf (stderr, "Illegal event found\n"); 
        printf("%d", d -> EventType); 
        exit(1);
    }
    free (d); // Release memory for event paramters
}

// event handler for generate events
void Generate1 (struct EventData *e, int IAT)
{
    struct EventData *d;
    struct Customer *NewCust;
    double ts;
    struct Generator *pGen;  // pointer to info on generator component
 
    if (e->EventType != GENERATE1) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
    printf ("Processing Generate1 event at time %f, Component=%d\n", CurrentTime(), e->CompID);
    P1Count ++;
    
    if (Component[e->CompID].ComponentType != GENERATOR) {fprintf(stderr, "bad componenet type\n"); exit(1);}
    // Get pointer to state information for generator component
    pGen = (struct Generator *) Component[e->CompID].Comp;
    printf("\nInter arr time: %f\n",pGen->IntArrTime);
    pGen->IntArrTime = IAT;
    
    // Create a new customer
    if ((NewCust=malloc (sizeof(struct Customer))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    NewCust->CreationTime = CurrentTime();
    NewCust->ExitTime = 0.0;    // undefined initially
    NewCust->next = NULL;       // undefined initially
    NewCust -> WaitTime = 0;
    NewCust -> type = 1;
    printf("\nPatient Type: %d\n",NewCust -> type);
    pGen -> Count ++;

    // Schedule arrival event at component connected to generator
    if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = ARRIVAL;
    d->Cust = NewCust;
    printf("%d", NewCust -> type);
    d->CompID = pGen->DestComp;
    ts = CurrentTime();
    Schedule (ts, d);
    
    // Schedule next generation event
    if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = GENERATE1;
    d->CompID = e->CompID;
    ts = P1Count * pGen->IntArrTime;       // need to modify to exponential random number
    Schedule (ts, d);
    printf("scheduled at %f\n", ts);  
    
}

void Generate2 (struct EventData *e, int IAT)
{
    struct EventData *d;
    struct Customer *NewCust;
    double ts;
    struct Generator *pGen;  // pointer to info on generator component
 
    if (e->EventType != GENERATE2) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
    printf ("Processing Generate event at time %f, Component=%d\n", CurrentTime(), e->CompID);
    
    if (Component[e->CompID].ComponentType != GENERATOR) {fprintf(stderr, "bad componenet type\n"); exit(1);}
    // Get pointer to state information for generator component
    pGen = (struct Generator *) Component[e->CompID].Comp;
    printf("\nInter arr time: %f\n",pGen->IntArrTime);
    pGen->IntArrTime = IAT;
    
    // Create a new customer
    if ((NewCust=malloc (sizeof(struct Customer))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    NewCust->CreationTime = CurrentTime();
    NewCust->ExitTime = 0.0;    // undefined initially
    NewCust->next = NULL;       // undefined initially
    NewCust -> WaitTime = 0;
    NewCust -> type = 2;
    printf("\nPatient Type: %d\n",NewCust -> type);
    pGen -> Count ++;

    // Schedule arrival event at component connected to generator
    if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = ARRIVAL;
    d->Cust = NewCust;
    printf("%d", NewCust -> type);
    d->CompID = pGen->DestComp;
    ts = CurrentTime();
    Schedule (ts, d);
    
    // Schedule next generation event
    if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = GENERATE2;
    d->CompID = e->CompID;
    ts = CurrentTime() + pGen->IntArrTime;       // need to modify to exponential random number
    Schedule (ts, d);
}

// event handler for arrival events
void Arrival (struct EventData *e)
{
    struct EventData *d;
    double ts;
    int Comp;                   // ID of component where arrival occurred
    struct Exit *pExit;         // pointer to info on exit component

    if (e->EventType != ARRIVAL) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
    printf ("Processing Arrival event at time %f, Component=%d\n", CurrentTime(), e->CompID);
    
    // processing depends on the type of component arrival occurred at
    if (Component[e->CompID].ComponentType == EXIT) {
        pExit = (struct Exit *) Component[e->CompID].Comp;
        (pExit->Count)++;       // number of exiting ßcustomers
        free (e->Cust);         // release memory of exiting customer
    }
    else if (Component[Comp].ComponentType == QUEUE_STATION) {
        // code for customer arrival at a queue station
    }
    else if (Component[Comp].ComponentType == FORK) {
        // code for customer arrival at a fork component
    }
    else {fprintf (stderr, "Bad component type for arrival event\n"); exit(1);}
}

///////////////////////////////////////////////////////////////////////////////////////
//////////// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////////////

int main (void)
{
    struct EventData *d;
    double ts;

    double IAT1 = 10;
    double IAT2 = 18;
    
    // create components
    MakeGenerator1 (0, IAT1, 1);
    MakeGenerator2 (0, IAT2, 1);
    //MakeGenerator2 (0, 18.0, 1);
    //MakeGenerator3 (0, 30.0, 1);
    //MakeGenerator4 (0, 60.0, 1);
    MakeExit (1);

    RunSim(120.0);
}
