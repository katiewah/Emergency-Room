#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "sim.h"


///////////////////////////////////////////////////////////////////////////////////////
//////////// MAIN PROGRAM
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char * argv[])
{

    const char* filename = argv[1];
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        printf("Error! opening file");
        exit(1);
    }
    int k = 0;
    int num_doctors;
    int num_staff;
    double cold_prob;
    double frac_prob;
    double breath_prob;
    double trauma_prob;
    double textarr[6] = {0};
     //finding number of components in the simulation
    char line[1000];
    while (fgets(line,1000,f) != NULL){
        if (k==0){
            num_doctors = atoi(line);
            textarr[0] = num_doctors;
        }
        if (k==1){
            num_staff = atoi(line);
            textarr[1] = num_staff;
        }
        if (k==2){
            cold_prob = atof(line);
            textarr[2] = cold_prob;
        }
        if (k==3){
            frac_prob = atof(line);
            textarr[3] = frac_prob;
        }
        if (k==4){
            breath_prob = atof(line);
            textarr[4] = breath_prob;
        }
        if (k==5){
            trauma_prob = atof(line);
            textarr[5] = trauma_prob;
        }
        k++;
    }
    fclose(f);

    struct EventData *d;
    double ts;

    int destC1[num_staff];
    int staff = 6;
    for (int i=0;i<num_staff;i++){
        destC1[i] = staff;
        staff++;
    }
    int destC2[num_doctors];
    int doctors = staff + num_staff;
    for (int i=0;i<num_doctors;i++){
        destC2[i] = doctors;
        doctors++;
    }
    int destC3[5];
    for (int i = 0;i<5;i++){
        destC3[i] = doctors;
        doctors++;
    }

    // create components

    ///////////////////////////////////////////////////////////////////////////////////////
    // this program creates:
    // - 4 generators, each one producing the different patient type
    // - 5 people at the check in counter
    // - 5 doctors
    // - 5 people at the check out counter
    ///////////////////////////////////////////////////////////////////////////////////////
    double probability_array[4];
    for (int i=0;i<4;i++){
        probability_array[i] = 60*textarr[i+2];
    }

    for (int b=0;b<4;b++){
        if (b==0){
            MakeGenerator1(0,(int)probability_array[b],5);
        }
        if (b==1){
            MakeGenerator2(1,(int)probability_array[b],5);
        }
        if (b==2){
            MakeGenerator3(2,(int)probability_array[b],5);
        }
        if (b==3){
            MakeGenerator4(3,(int)probability_array[b],5);
        }
    }
    MakeFork(5,num_staff,destC1);          //destC1, dest C2, and destC3 are arrays of the different components to which the fork leads
    int i = 6;
    int bound = i + num_staff;
    for (i;i<bound;i++){
        MakeCheckInOut(i,5,bound+1);
    }
    MakeFork(bound+1,num_doctors,destC2);
    i = bound + 2;
    bound = i + num_doctors;
    for (i; i<bound;i++){
        MakeDoctor(i,15,bound+1);
    }
    MakeFork(bound+1,5,destC3);
    i = bound + 2;
    bound = i + 5;
    for (i; i<bound;i++){
        MakeCheckInOut(i,5,bound+1);
    }
    MakeExit(bound+1);

    RunSim(1440.0);                          //run for 1440 to get a 24 hour simulation
    PrintResults();
}
