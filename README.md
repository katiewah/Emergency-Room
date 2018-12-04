# Emergency-Room
Author          : Ashwin Haritsa and Katie Wah
Created         : November 23, 2018
Last Modified   : December 4, 2018
Assignment	: Homework 5 - Project

Affiliation          : Georgia Institute of Technology


Description
-------------

This assignment involves building a simulation software model for an
emergency room. This simulation software is divided into 2 independent
component files: the Simulation Library and the Configuration File. The 
Configuration File sets the layout and the process flow within the 
hypothetical emergency room. The file is responsible for scheduling
future events in either chronological order or by priority based on
what type of queue the patient is being entered into and running the 
simulation software. The Simulation Library is what allows the software 
to run. It contains all the functions to generate different patients 
and the different components (generator, queue stations, check-in/out 
desk, doctors, forks, and exit). In addition, it is responsible for 
handling different types of events (Generate or Arrival at component) 
and printing output statistics. The third file (main.c/mainRand.c) is
responsible for calling and executing the functions defined in the 
two files described above. The main file reads in the text file with
the number of doctors, staff members, and inter-arrival times and
either generates that number of doctors/staff or assigns each patient
their respective inter-arrival time.

Note: There are two versions of our software. Files with the word "Rand"
attached to the end of the file name is used to run the software with
exponentially distributed inter-arrival times. We chose to model
inter-arrival times exponentially because that is the closest estimate
to how an actual emergency room operates in the real world.


Installation
------------

To install the discrete time interval program, simply run

    gcc main.c engine.c application.c -std=c99 -o test
    
To install the exponential time interval program, simply run
    gcc mainRand.c engineRand.c applicationRand.c -std=c99 -o test
    
Note: The -std=c99 flag is required on some C compilers
to allow variable declarations at arbitrary locations in
a function.

Execution
-----------

Assuming your executable is called “test”, run it using

    ./test emergency.txt
    
Note: emergency.txt is the text file which includes the number 
of doctors, number of staff members, and inter-arrival times of 
each type of patient.
