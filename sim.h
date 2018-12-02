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

// Call this procedure to run the simulation indicating time to end simulation
void RunSim (double EndTime);

// Schedule an event with timestamp ts, event parameters *data
void Schedule (double ts, void *data, int custType, int nextComp);

// This function returns the current simulation time
double CurrentTime (void);

void changeSchedulePQ (int type, double ts, void *data, int add);

double PrioritySchedule (double ts, void *data, int custType, int nextComp);

//double returnsTime(double ts);

//void bubbleSort(struct Event *start);

//void swap(struct Event *a, struct Event *b);

//
// Function defined in the simulation application called by the simulation engine
//
//  Event handler function: called to process an event
void EventHandler (void *data, double done);

double end(double EndTime);

