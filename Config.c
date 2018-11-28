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
// Data srtucture for an event; note this is designed to be independent of the application domain.
// Each event can have parameters defined within the spplication. We want the simulation engine
// not to have to know the number or types of these parameters, since they are dependent on the
// application, and we want To keep the engine independent of the application. To address this problem
// each event only contains a single parameter, a pointer to the application defined parameters (AppData).
// The simulation engine only knows it has a pointer to the event parameters, but does not know the
// structure (number of parameters or their type) of the information pointed to.
// This way the event can have application-defined information, but the simulation engine need not
// know the number or type of the application-defined parameters.
//
struct Event {
    double timestamp;       // event timestamp
    void *AppData;          // pointer to application defined event parameters
    struct Event *Next;     // priority queue pointer
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
struct Event *Remove (void);

// Function to read the configuration file with all information on number of staff people
void read_file (char* filename);

/////////////////////////////////////////////////////////////////////////////////////////////
// Simulation Engine Functions Internal to this module
/////////////////////////////////////////////////////////////////////////////////////////////

// Remove smallest timestamped event from FEL, return pointer to this event
// return NULL if FEL is empty
struct Event *Remove (void)
{
    struct Event *e;

    if (FEL.Next==NULL) return (NULL);
    e = FEL.Next;       // remove first event in list
    FEL.Next = e->Next;
    return (e);
}

// Print timestamps of all events in the event list (used for debugging)
void PrintList (void)
{
    struct Event *p;

    printf ("Event List: ");
    for (p=FEL.Next; p!=NULL; p=p->Next) {
        printf ("%f ", p->timestamp);
    }
    printf ("\n");
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


// read file and get number of doctors, staff, and probabilities of each type of patient
void read_file(char* filename) {
    //opening file to read
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    int i = 0;
    int num_doctors;
    int num_staff;
    double cold_prob;
    double frac_prob;
    double breath_prob;
    double trauma_prob;

    //finding number of components in the simulation
    char line[1000];
    while (fgets(line,1000,f) != NULL){
        if (i==0){
            num_doctors = atoi(line);
        }
        if (i==1){
            num_staff = atoi(line);
        }
        if (i==2){
            cold_prob = atof(line);
        }
        if (i==3){
            frac_prob = atof(line);
        }
        if (i==4){
            breath_prob = atof(line);
        }
        if (i==5){
            trauma_prob = atof(line);
        }
        i++;
    }


    printf("%d %d %.2f %.2f %.2f %.2f\n", num_doctors,num_staff,cold_prob,frac_prob,breath_prob,trauma_prob);

}

// Return current simulation time
double CurrentTime (void)
{
    return (Now);
}

// Schedule new event in FEL
// queue is implemented as a timestamp ordered linear list

void Schedule (double ts, void *data)
{
    struct Event *e, *p, *q;

    // create event data structure and fill it in
    if ((e = malloc (sizeof (struct Event))) == NULL) exit(1);
    e->timestamp = ts;
    e->AppData = data;

    // insert into priority queue
    // p is lead pointer, q is trailer
    for (q=&FEL, p=FEL.Next; p!=NULL; p=p->Next, q=q->Next) {
        if (p->timestamp >= e->timestamp) break;
        }
    // insert after q (before p)
    e->Next = q->Next;
    q->Next = e;
}

// Function to execute simulation up to a specified time (EndTime)
void RunSim (double EndTime)
{
    struct Event *e;

    printf ("Initial event list:\n");
    PrintList ();

    // Main scheduler loop
    while ((e=Remove()) != NULL) {
        Now = e->timestamp;
        if (Now > EndTime) break;
        EventHandler(e->AppData);
        free (e);   // it is up to the event handler to free memory for parameters
        PrintList ();
    }
}


int main(int argc, char* argv[]) {
    char* filename = argv[1];
    read_file(filename);
}

