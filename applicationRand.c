#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "simRand.h"

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Variables used for data Generation
//
/////////////////////////////////////////////////////////////////////////////////////////////

int P1Count = 1;
int P2Count = 1;
int P3Count = 1;
int P4Count = 1;
int numGenPatients = 0;
int numDeathsAtCheckIn = 0;
int numDeaths = 0;
int numPatientsSaved = 0;
int numPatientsER = 0;
int sixhours = 0;
int DRcount = 0;
int conciergeCount = 0;

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
    int nextComp;
};

// Linked List of customers (FIFO Queue)
typedef struct FIFOQueue {
    struct Customer* first;     // pointer to first customer in queue
    struct Customer* last;      // pointer to last customer in queue
    struct Customer* temp;    
} FIFOQueue;

struct PrioQ {
    struct Customer* first;     // pointer to first customer in queue
    struct Customer* last;      // pointer to last customer in queue
    struct Customer* next;    
};

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Data Structures for Components
//
/////////////////////////////////////////////////////////////////////////////////////////////

// maximum number of components that can be defined for the simulation
#define MAXCOMPONENTS    100

// Component types
#define     GENERATOR        0
#define     GENERATOR2       1
#define     GENERATOR3       2
#define     GENERATOR4       3
#define     FORK             4
#define     QUEUE_STATION    5
#define     EMERROOM         6
#define     EXIT             7
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
struct CheckInOut {
    double avgServiceTime;         //service time
    int DestComp;                  // ID of next component customers are sent to
    struct FIFOQueue* fifo;        //fifo queue at every station
    double sTime;                  //scheduled time of the last scheduled event
    double wait;                   //average wait time of a customer in that station
    int count;                     //number of customers who go though that station
} CheckInOut;

struct Doctor {
    double avgServiceTime;         //service time
    int DestComp;                  // ID of next component customers are sent to
    struct PrioQ* pq;        //fifo queue at every station
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

////////////////////////////////////////////////////
//
//creates random double that exists between 0 and 1
//
////////////////////////////////////////////////////
double urand(void) {
    double r = (double) rand() / RAND_MAX;
    return (r);
}

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
    //printf("%f\n", CurrentTime());
    Schedule ((-IntTime)*(log(1.0 - urand())), d,1,4);  // should use random draw here
    numGenPatients++;
    return *p;
}

struct Generator MakeGenerator2 (int GenID, double IntTime, int DestID)
{   struct Generator* p;
    struct EventData* d;
    compCount++;
    
    // Add component to master list; Caller is responsible for handling set up errors
    Component[GenID].ComponentType = GENERATOR2;
    printf ("Creating Generator Component, ID=%d, Interarrival time=%f, Destination=%d\n", GenID, IntTime, DestID);
    
    // Allocate space for component, fill in parameters
    if ((p = malloc (sizeof(struct Generator))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    p->IntArrTime = IntTime;
    p->DestComp = DestID;
    p-> Count = 0;
    Component[GenID].Comp = p;
    
    // schedule initial, first generator event for this component for patient of type 1
    if ((d = malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = GENERATE2;
    d->Cust = NULL;
    d->CompID = GenID;
    //printf("%f\n", CurrentTime());
    Schedule ((-IntTime)*(log(1.0 - urand())), d,2,4);  // should use random draw here
    numGenPatients++;
    return *p;
}

struct Generator MakeGenerator3 (int GenID, double IntTime, int DestID)
{   struct Generator* p;
    struct EventData* d;
    compCount++;
    
    // Add component to master list; Caller is responsible for handling set up errors
    Component[GenID].ComponentType = GENERATOR3;
    printf ("Creating Generator Component, ID=%d, Interarrival time=%f, Destination=%d\n", GenID, IntTime, DestID);
    
    // Allocate space for component, fill in parameters
    if ((p = malloc (sizeof(struct Generator))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    p->IntArrTime = IntTime;
    p->DestComp = DestID;
    p-> Count = 0;
    Component[GenID].Comp = p;
    
    // schedule initial, first generator event for this component for patient of type 1
    if ((d = malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = GENERATE3;
    d->Cust = NULL;
    d->CompID = GenID;
    //printf("%f\n", CurrentTime());
    Schedule ((-IntTime)*(log(1.0 - urand())), d,3,4);  // should use random draw here
    numGenPatients++;
    return *p;
}

struct Generator MakeGenerator4 (int GenID, double IntTime, int DestID)
{   struct Generator* p;
    struct EventData* d;
    compCount++;
    
    // Add component to master list; Caller is responsible for handling set up errors
    Component[GenID].ComponentType = GENERATOR4;
    printf ("Creating Generator Component, ID=%d, Interarrival time=%f, Destination=%d\n", GenID, IntTime, DestID);
    
    // Allocate space for component, fill in parameters
    if ((p = malloc (sizeof(struct Generator))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    p->IntArrTime = IntTime;
    p->DestComp = DestID;
    p-> Count = 0;
    Component[GenID].Comp = p;
    
    // schedule initial, first generator event for this component for patient of type 1
    if ((d = malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = GENERATE4;
    d->Cust = NULL;
    d->CompID = GenID;
    //printf("%f\n", CurrentTime());
    Schedule ((-IntTime)*(log(1.0 - urand())), d,4,4);  // should use random draw here
    numGenPatients++;
    return *p;
}

// Create an CheckInOut Component with identifier ExitID
struct CheckInOut MakeCheckInOut (int StationID, double avgServiceTime, int DestID) {
    struct CheckInOut* s;
    //struct FIFOQueue* fifo;
    compCount++;
    conciergeCount++;

    // Add component to master list; Caller is responsible for handling set up errors
    Component[StationID].ComponentType = QUEUE_STATION;
    printf ("Creating Station Component, ID=%d, avgServiceTime=%f, Destination=%d\n", StationID, avgServiceTime, DestID);

    // Allocate space for component, fill in parameters
    if ((s = malloc (sizeof(struct CheckInOut))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    s -> avgServiceTime = avgServiceTime;
    s -> DestComp = DestID;
    s -> sTime = 0.0;
    s ->count = 0;
    s -> wait = 0;

    FIFOQueue* fifo = (FIFOQueue*) malloc(sizeof(FIFOQueue));
    
    Component[StationID].Comp = s;
    return *s;
}

// Create an CheckInOut Component with identifier ExitID
struct Doctor MakeDoctor (int erID, double avgServiceTime, int DestID) {
    struct Doctor* s;
    compCount++;
    DRcount++;

    // Add component to master list; Caller is responsible for handling set up errors
    Component[erID].ComponentType = EMERROOM;
    printf ("Creating Station Component, ID=%d, avgServiceTime=%f, Destination=%d\n", erID, avgServiceTime, DestID);

    // Allocate space for component, fill in parameters
    if ((s = malloc (sizeof(struct Doctor))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    s -> avgServiceTime = avgServiceTime;
    s -> DestComp = DestID;
    s -> sTime = 0.0;
    s ->count = 0;
    s -> wait = 0;
    
    Component[erID].Comp = s;
    return *s;
}

struct Fork MakeFork (int StationID, int distribution, int* fieldArr) {
    struct Fork* f;
    compCount++;
    double prob[distribution];
    double acc = 1.0/distribution;
    for (int i = 0; i < distribution-1; i++) {
        prob[i] = acc;
    }
    prob[distribution - 1] = 1 - (acc * (distribution - 1));

    // Add component to master list; Caller is responsible for handling set up errors
    Component[StationID].ComponentType = FORK;
    printf ("Creating Fork Component, ID=%d, fieldArr=%d\n", StationID, fieldArr[0]);

    // Allocate space for component, fill in parameters
    if ((f = malloc (sizeof(struct Fork))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    f -> distribution = distribution;
    f -> probArr = prob;
    f -> fieldArr = fieldArr;
    Component[StationID].Comp = f;
    return *f;
}

// Create an Exit Component with identifier ExitID
struct Exit MakeExit (int ExitID)
{   struct Exit *p;
    
    printf ("Creating Exit Component, ID=%d\n", ExitID);
    Component[ExitID].ComponentType = EXIT;
    
    // Allocate space for component, fill in parameters
    if ((p=malloc (sizeof(struct Exit))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    p->Count = 0;
    Component[ExitID].Comp = p;
    return *p;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Function prototypes
//
/////////////////////////////////////////////////////////////////////////////////////////////

// prototypes for event handlers
void Generate1 (struct EventData *e);    // generate new customer 
void Arrival (struct EventData *e, double done);     // arrival at a component



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
void EventHandler (void *data, double done)
{
    struct EventData *d;
    
    // coerce type so the compiler knows the type of information pointed to by the parameter data.
    d = (struct EventData *) data;
    // call an event handler based on the type of event
    if (d->EventType == GENERATE1) Generate1 (d);
    else if (d->EventType == GENERATE2) Generate1 (d);
    else if (d->EventType == GENERATE3) Generate1 (d);
    else if (d->EventType == GENERATE4) Generate1 (d);
    else if (d->EventType == ARRIVAL) Arrival (d, done);
    else {
        fprintf (stderr, "Illegal event found\n"); 
        exit(1);
    }
    free (d); // Release memory for event paramters
}

// event handler for generate events
void Generate1 (struct EventData *e)
{
    struct EventData *d;
    struct Customer *NewCust;
    double ts;
    struct Generator *pGen;  // pointer to info on generator component
 
    if (e->EventType != GENERATE1) {
        if (e->EventType != GENERATE2) {
            if (e->EventType != GENERATE3) {
                if (e->EventType != GENERATE4) {
                    fprintf (stderr, "Unexpected event type\n"); 
                    exit(1);
                }
            }
        }
    }

    printf ("Processing Generate1 event at time %f, Component=%d\n", CurrentTime(), e->CompID);
    P1Count ++;
    
    if (Component[e->CompID].ComponentType != GENERATOR) {
        if (Component[e->CompID].ComponentType != GENERATOR2) {
            if (Component[e->CompID].ComponentType != GENERATOR3) {
                if (Component[e->CompID].ComponentType != GENERATOR4) {
                    fprintf(stderr, "bad componenet type\n"); 
                    exit(1);
                }
            }
        }
    }

    // Get pointer to state information for generator component
    pGen = (struct Generator *) Component[e->CompID].Comp;
    printf("Inter arr time: %f\n",pGen->IntArrTime);
    
    // Create a new customer
    if ((NewCust=malloc (sizeof(struct Customer))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    NewCust->CreationTime = CurrentTime();
    NewCust->ExitTime = 0.0;    // undefined initially
    NewCust->next = NULL;       // undefined initially
    NewCust -> WaitTime = 0;
    if ((Component[e->CompID].ComponentType == GENERATOR) && (e->EventType == GENERATE1)) {
        NewCust -> type = 1;
    }
    if ((Component[e->CompID].ComponentType == GENERATOR2) && (e->EventType == GENERATE2)) {
        NewCust -> type = 2;
    }
    if ((Component[e->CompID].ComponentType == GENERATOR3) && (e->EventType == GENERATE3)) {
        NewCust -> type = 3;
    }
    if ((Component[e->CompID].ComponentType == GENERATOR4) && (e->EventType == GENERATE4)) {
        NewCust -> type = 4;
    }
    printf("\nPatient Type: %d\n",NewCust -> type);
    pGen -> Count ++;

    // Schedule arrival event at component connected to generator
    if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    d->EventType = ARRIVAL;
    d->Cust = NewCust;

    //printf("%d", NewCust -> type);
    d->CompID = pGen->DestComp;
    //printf("destination Comp: %d\n", pGen->DestComp);
    //printf("Comp type %d\n",Component[d->CompID].ComponentType);
    ts = CurrentTime();
    Schedule (ts, d, d->Cust ->type, d->CompID);
    
    // Schedule next generation event
    if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
    if (Component[e->CompID].ComponentType == GENERATOR) {
        d->EventType = GENERATE1;
    }
    if (Component[e->CompID].ComponentType == GENERATOR2) {
        d->EventType = GENERATE2;
    }
    if (Component[e->CompID].ComponentType == GENERATOR3) {
        d->EventType = GENERATE3;
    }
    if (Component[e->CompID].ComponentType == GENERATOR4) {
        d->EventType = GENERATE4;
    }
    
    d->CompID = e->CompID;
    ts = CurrentTime() + (-(pGen->IntArrTime)*(log(1.0 - urand())));       // need to modify to exponential random number
    Schedule (ts, d, Component[e->CompID].ComponentType, 4);
    //printf("scheduled at %f\n", ts);     
    numGenPatients++;
}

// event handler for arrival events
void Arrival (struct EventData *e, double done)
{
    struct EventData *d;
    struct EventData *k;
    double ts;
    int Comp;                   // ID of component where arrival occurred
    struct Exit *pExit;         // pointer to info on exit component
    struct CheckInOut* pCheckInOut;   // pointer to info at station 
    struct Doctor *pER;
    struct Fork* pFork;
    double aTime;

    if (e->EventType != ARRIVAL) {fprintf (stderr, "Unexpected event type\n"); exit(1);}
    //printf("component type: %d\n",Component[Comp].ComponentType);
    printf ("Processing Arrival event at time %f, Component=%d\n", CurrentTime(), e->CompID);
    
    // processing depends on the type of component arrival occurred at
    if (Component[e->CompID].ComponentType == EXIT) {
        pExit = (struct Exit *) Component[e->CompID].Comp;
        (pExit->Count)++;       // number of exiting ßcustomers
        free (e->Cust);         // release memory of exiting customer
        numPatientsSaved ++;
    }
    else if (Component[e->CompID].ComponentType == QUEUE_STATION) {

        // code for customer arrival at a queue station
        pCheckInOut = (struct CheckInOut*) Component[e -> CompID].Comp;
        (pCheckInOut -> count) ++;

        if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = ARRIVAL;  
        d -> Cust = e -> Cust;   
        d->CompID = pCheckInOut -> DestComp;
        
        if ((d -> Cust -> CreationTime >= 0) && (d -> Cust -> type == 3) && (CurrentTime() - d -> Cust -> CreationTime >= 90)) {
                free(d);
                numDeathsAtCheckIn++;
        } else if ((d -> Cust -> CreationTime >= 0) && (d -> Cust -> type == 4) && (CurrentTime() - d -> Cust -> CreationTime >= 60)) {
                free(d);
                numDeathsAtCheckIn++;
        } else {
            double u = (pCheckInOut -> avgServiceTime);
            //if (CurrentTime() + (pStation -> avgServiceTime) > pStation -> sTime) {
            if (CurrentTime() >= pCheckInOut -> sTime) {
                double aTime = CurrentTime() + (pCheckInOut -> avgServiceTime)*-1*(log(1.0 - urand()));          //exponentian random varialbe
                Schedule(aTime, d, d->Cust ->type, d->CompID);
                pCheckInOut -> sTime = aTime;
            } else {
                double aTime = pCheckInOut -> sTime + (pCheckInOut -> avgServiceTime)*-1*(log(1.0 - urand()));             //exponential random varialbe
                Schedule(aTime, d, d->Cust ->type,  d->CompID); 
                if (aTime > done) {
                    pCheckInOut -> wait += (done - CurrentTime());
                } else {
                    pCheckInOut -> wait += (pCheckInOut -> sTime) - CurrentTime();
                }
                pCheckInOut -> sTime = aTime;
                e -> Cust -> WaitTime += (pCheckInOut -> sTime) - CurrentTime();
            }
        }    
            
    }
    else if (Component[e -> CompID].ComponentType == EMERROOM) {

        pER = (struct Doctor*) Component[e -> CompID].Comp;
        (pER -> count) ++;

        if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = ARRIVAL;  
        d -> Cust = e -> Cust;   
        d->CompID = pER -> DestComp;
        //printf("this is the dest comp: %d\n", d->CompID);
        int fixTime;
        if ((CurrentTime() - d -> Cust -> CreationTime > 360) && (d -> Cust -> type != 3) && (d -> Cust -> type != 4)) {
            sixhours++;
        }
        //aTime = (pER -> avgServiceTime)*-1*(log(1.0 - urand()));
        //if (CurrentTime() + (pStation -> avgServiceTime) > pStation -> sTime) {
        if (d -> Cust -> type == 2) {
            aTime = -30*(log(1.0 - urand()));
            if (CurrentTime() >= pER-> sTime) {
                //printf("\n\n\nhere %f     %f\n\n\n", pER -> sTime, CurrentTime());
                Schedule(CurrentTime() + aTime, d, d->Cust ->type, d->CompID);
                //pER -> sTime = CurrentTime() + (-u*(log(1.0 - urand())));
                pER -> sTime = CurrentTime() + aTime;
            } else {
                double add = PrioritySchedule(aTime, d, d->Cust ->type, d->CompID, aTime);

                //printf("here2");
                pER -> sTime = aTime + add; //this is incorrect
            }
        } else if (d -> Cust -> type == 3) {
            printf("This is the patients creation time vs current time: %f     %f", d -> Cust -> CreationTime, CurrentTime());
            aTime = -60*(log(1.0 - urand()));
            if (CurrentTime() - d -> Cust -> CreationTime > 90) {
                printf("\nDEATH!!\n\n\n\n\n");
                free(d);
                numDeaths++;
            } else {
                d -> Cust -> CreationTime = -1;
                if (CurrentTime() >= pER-> sTime) {
                    //printf("\n\n\nhere %f     %f\n\n\n", pER -> sTime, CurrentTime());
                    Schedule(CurrentTime() + aTime, d, d->Cust ->type, d->CompID);
                    //pER -> sTime = CurrentTime() + (-u*(log(1.0 - urand())));
                    pER -> sTime = CurrentTime() + aTime;
                } else {
                    double add = PrioritySchedule(aTime, d, d->Cust ->type, d->CompID, aTime);
                    //printf("here2");
                    pER -> sTime = aTime + add; //this is incorrect
                }
            }
        } else if (d -> Cust -> type == 4) {
            aTime = -90*(log(1.0 - urand()));
            printf("This is the patients creation time vs current time: %f     %f", d -> Cust -> CreationTime, CurrentTime());
            if (CurrentTime() - d -> Cust -> CreationTime > 60) {
                printf("\nDEATH!!\n\n\n\n\n");
                free(d);
                numDeaths++;
            } else {
                d -> Cust -> CreationTime = -1;
                if (CurrentTime() >= pER-> sTime) {
                    //printf("\n\n\nhere %f     %f\n\n\n", pER -> sTime, CurrentTime());
                    Schedule(CurrentTime() + aTime, d, d->Cust ->type, d->CompID);
                    //pER -> sTime = CurrentTime() + (-u*(log(1.0 - urand())));
                    pER -> sTime = CurrentTime() + aTime;
                } else {
                    double add = PrioritySchedule(aTime, d, d->Cust ->type, d->CompID, aTime);

                    //printf("here2");
                    pER -> sTime = aTime + add; //this is incorrect
                }
            }     
        } else {
            aTime = -15*(log(1.0 - urand()));
            if (CurrentTime() >= pER -> sTime) {
                Schedule(CurrentTime() + aTime, d, d->Cust ->type, d->CompID);
                //printf("scheduled event at: %f\n", CurrentTime());
                //pER -> sTime = CurrentTime() + (-u*(log(1.0 - urand())));
                //pER -> sTime = CurrentTime() + (pER-> avgServiceTime);
                pER -> sTime = CurrentTime() + aTime;
            } else {
                //double aTime = (-u*(log(1.0 - urand())));             //exponential random varialbe
                Schedule(pER -> sTime, d, d->Cust ->type, d->CompID); 
                //printf("scheduled this at: %f\n", pER -> sTime);

                if (pER -> sTime > done) {
                    pER -> wait += (done - CurrentTime());
                } else {
                    pER -> wait += (pER -> sTime) - CurrentTime();
                }
                pER -> sTime = pER -> sTime + (pER -> avgServiceTime);
                e -> Cust -> WaitTime += (pER -> sTime) - CurrentTime();
            }
        }
            
    }
    else if (Component[e -> CompID].ComponentType == FORK) {
        // code for customer arrival at a fork component
        pFork = (struct Fork*) Component[e -> CompID].Comp;
        if ((d=malloc (sizeof(struct EventData))) == NULL) {fprintf(stderr, "malloc error\n"); exit(1);}
        d->EventType = ARRIVAL;  
        d -> Cust = e -> Cust;   

        //create random number
        for (int i = 0; i < 3; i++) {
            //#printf("comp array %d\n", pFork -> fieldArr[i]);
            //#Component[(pFork -> fieldArr[i])].Comp -> sTime = 0.0;
            //printf("comp array sTime: %f\n", Component[(pFork -> fieldArr[i])].Comp -> sTime);
        }
        double r = (double)rand() / RAND_MAX;
        int p = 0; while (pFork -> probArr[p] != 0) { p++; }
        int end = (int)(sizeof(pFork -> probArr) / sizeof(pFork -> probArr[0])) + 1;
        double incProb = pFork -> probArr[0];             //incrementing probability
        for (int i = 1; i < pFork -> distribution+1; i++) { //change int distribution
            if (r <= incProb) {
                d->CompID = pFork -> fieldArr[i-1]; //will have to change destination
                Schedule(CurrentTime(), d, d->Cust ->type, pER -> sTime);
                break;
            } else { 
                incProb += pFork -> probArr[i];
            }
        }
    }
    else {fprintf (stderr, "Bad component type for arrival event\n"); exit(1);}
}

///////////////////////////////////////////////////////////////////////////////////////
//////////// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////////////

int main (void)
{   
    srand(time(NULL));
    struct EventData *d;
    double ts;

    double IAT1 = 10;
    double IAT2 = 18;
    int destC1[] = {6,7,8,9,10};
    int destC2[] = {12,13,14,15,16};
    int destC3[] = {17,18,19,20,21};
    
    // create components

    ///////////////////////////////////////////////////////////////////////////////////////
    // this program creates:
    // - 4 generators, each one producing the different patient type
    // - 5 people at the check in counter
    // - 5 doctors
    // - 5 people at the check out counter
    ///////////////////////////////////////////////////////////////////////////////////////
    MakeGenerator1 (0, 6, 5);               //these numbers 6,18,30,and 60 should be varied to account for the holidays portion of analysis
    MakeGenerator2 (1, 18, 5);
    MakeGenerator3 (2, 30, 5);
    MakeGenerator4 (3, 60, 5);
    MakeFork(5,5,destC1);                   //destC1, dest C2, and destC3 are arrays of the different components to which the fork leads
    MakeCheckInOut (6, 5, 11);              //the middle number "5" should NEVER be changed for CheckInOuts
    MakeCheckInOut (7, 5, 11);              //what should vary is the number of people at the Check In/Out stations (make more MakeCheckInOuts())
    MakeCheckInOut (8, 5, 11);
    MakeCheckInOut (9, 5, 11);
    MakeCheckInOut (10, 5, 11);
    MakeFork(11,5,destC2);
    MakeDoctor(12,15,17);                   //the middle number "15" should NEVER be changed for MakeDoctors
    MakeDoctor(13,15,17);
    MakeDoctor(14,15,17);
    MakeDoctor(15,15,17);
    MakeDoctor(16,15,17);
    MakeFork(17,5,destC3);
    MakeCheckInOut (18, 5, 23);             //the middle number "5" should NEVER be changed for CheckInOuts
    MakeCheckInOut (19, 5, 23);
    MakeCheckInOut (20, 5, 23);
    MakeCheckInOut (21, 5, 23);
    MakeCheckInOut (22, 5, 23);
    MakeExit (23);

    RunSim(1440.0);                          //run for 1440 to get a 24 hour simulation
    printf("\n\nnum Patients who are generated: %d", numGenPatients);
    printf("\ndeaths at Check In: %d", numDeathsAtCheckIn);
    printf("\ndeaths at ER: %d", numDeaths);
    printf("\nsaved: %d", numPatientsSaved);
    printf("\nwaited longer than 6 hours: %d", sixhours);
    printf("\nFines: %d", ((numDeaths+numDeathsAtCheckIn)*100) + (sixhours*5));

    printf("\n\ndoctors: %d", DRcount);
    printf("\npeople at front desk: %d", conciergeCount);
    printf("\nDoctor Cost: %d", DRcount*250);
    printf("\nFront Desk Cost: %d", conciergeCount*100);
    printf("\nStaff Cost: %d", (DRcount*250) + (conciergeCount*100));
    printf("\n\n--------------");
    printf("\nTOTAL COST: %d", (DRcount*250) + (conciergeCount*100) + ((numDeaths+numDeathsAtCheckIn)*100) + (sixhours*5));
    printf("\n--------------");

}

//counting "saved" based on free() components, but maybe checkout
