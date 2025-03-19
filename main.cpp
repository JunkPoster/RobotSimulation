/*============================================================================*
   Project: Robot Evolution SimulationController
    Author: Ian Featherston
    Course: CISP400 - Object Oriented Programming in C++
      Date: 05/07/2024 - 05/16/2024
            - Revised on 03/18/2025
-------------------------------------------------------------------------------              
      Desc: This program simulates a population of Robots that evolve over 
            time. Each robot contains a set of Genes that determine their
            navigational behavior. 
            
            They navigate a grid that's filled with batteries, and
            they must collect as many batteries as possible before using up 
            their energy. Their performance is based on how many batteries 
            they're able to collect, and then they reproduce to pass on their 
            genes to the next generation, combining their genes with their 
            respective mate. 
            
            The simulation runs for a set number of generations, and the 
            performance metrics for each generation is evaluated and 
            compared at the end.

     Notes: 
            - Many of the settings can be adjusted in the SimulationSettings.h
              file in the sim_modules/sim/ directory.

This was my final project for CISP400, "Object Oriented Programming in C++." 
It's a simulation of a Robot population that evolves over time. The Robots
have a set of Genes that determine their behavior, and they can move around
a grid, collecting batteries to increase their fitness. The Robots can also
reproduce, passing on their Genes to their offspring. The simulation runs for
a set number of generations, and the population is evaluated at the end.

We were required to submit it as one file, but I've refactored it to be 
separated. If it seems like things are out of place or inheritance is wrong,
that's probably why.

I've tried my best to steer clear of using Macros, but I
used one to supplement the Logger to pass '__PRETTY_FUNCTION__' as 'FUNC'. 
I've also used a few global functions as helpers to various classes in place 
of macros, but I've tried to keep them to a minimum.

NOTE: I originally used raw pointers to store the population of Robot objects.
When checking all Logging activity I noticed that the vast majority of
classes/structs were NOT being destroyed, so I switched to using a vector of
shared_ptrs to avoid memory issues. This actually resulted in a small decrease
in runtime performance, but it *should* be more stable.

*============================================================================*/

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

// Utilities
#include "utilities/logger/Logger.h"
#include "utilities/FormatText.h"
#include "utilities/DateClass.h"

// SimulationController-Related Classes
#include "sim_modules/sim/SimulationMenu.h"

using namespace std;

/*================================<PROTOTYPES>===============================*/

void SimulationStarter();
void ProgramGreeting();

/*==================================<MAIN>===================================*/
int main() {
    Logger log(FUNC);
    srand((unsigned)time(NULL));

    ProgramGreeting();

    // Begin the simulation!
    SimulationStarter();

    cout << COLOR("\n\nProgram Ended\n", RED, BRIGHT);
    return 0;
}


// Begins the SimulationController. Separated into a standalone function so the Logger
// displays an accurate an sequence of events when closing.
void SimulationStarter() {
    Logger log(FUNC);

    SimulationMenu sim;
    sim.mainMenu();
}


void ProgramGreeting() {
    Logger log(FUNC);

    const string projName = COLOR("FinalProj.cpp", YELLOW);
    Date date;
    cout << "+-----------------------------------+\n"
        << "| " << projName << ", by Ian Featherston |\n"
        << "| CISP 400, " << date.getDate() << "              |\n"
        << "+-----------------------------------+\n";
}
