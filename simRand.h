//
//  Sample Discrete Event Simulation
//
//  Created by Richard Fujimoto on 9/24/17.
//  Copyright © 2017 Richard Fujimoto. All rights reserved.
//

//
// Application Independent Simulation Engine Interface
//
//
// Function defined in the simulation engine called by the simulation application
//

// structures of components
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

// Call this procedure to run the simulation indicating time to end simulation
void RunSim (double EndTime);

// Schedule an event with timestamp ts, event parameters *data
void Schedule (double ts, void *data, int custType, int nextComp);

// This function returns the current simulation time
double CurrentTime (void);

void changeSchedulePQ (int type, double ts, void *data, int add);

double PrioritySchedule (double ts, void *data, int custType, int nextComp, double aTime);

//double returnsTime(double ts);

//void bubbleSort(struct Event *start);

//void swap(struct Event *a, struct Event *b);


// functions to generate components
struct Generator MakeGenerator1 (int GenID, double IntTime, int DestID);
struct Generator MakeGenerator2 (int GenID, double IntTime, int DestID);
struct Generator MakeGenerator3 (int GenID, double IntTime, int DestID);
struct Generator MakeGenerator4 (int GenID, double IntTime, int DestID);
struct CheckInOut MakeCheckInOut (int StationID, double avgServiceTime, int DestID);
struct Doctor MakeDoctor (int erID, double avgServiceTime, int DestID);
struct Fork MakeFork (int StationID, int distribution, int* fieldArr);
struct Exit MakeExit (int ExitID);
void PrintResults();
//
// Function defined in the simulation application called by the simulation engine
//
//  Event handler function: called to process an event
void EventHandler (void *data, double done);

double end(double EndTime);
