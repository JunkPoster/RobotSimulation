/*============================================================================*
File: StatsRobots.cpp
Desc: Implementation of the RobotStats struct
*============================================================================*/

#include "StatsRobots.h"

double RobotStats::getAvgFitness() const {
    if (rounds == 0) return 0;
    return fitness / rounds;
}

double RobotStats::getAvgTurns() const {
    if (rounds == 0) return 0;
    return turns / rounds;
}

// Copy Constructor
RobotStats::RobotStats(const RobotStats& copy) { 
    *this = copy; 
}

RobotStats& RobotStats::operator=(const RobotStats& copy) {
    fitness = copy.fitness;
    turns = copy.turns;
    rounds = copy.rounds;
    generation = copy.generation;
    mutations = copy.mutations;
    return *this;
}