/*============================================================================*
File: Robot.h
Desc: This file contains the Robot class, which is responsible for managing
      the Robot's Genes, Energy, and Stats.
*============================================================================*/

#pragma once

#include "genes/GeneManager.h"
#include "../stats/StatsRobots.h"

using namespace std;

/*    Robot Class    */
class Robot {

    RandNo rand;
    GeneManager genes;
    RobotStats stats;

    int energy;
    bool alive = true;

public:
    Robot();
    Robot(const Robot& copy);

    char processSensorData(array<char, 4> area);
    void checkHealth();
    void startRound();

    // Setters
    void increaseTurns();
    void increaseMutations();
    void expendEnergy(int n);
    void setGeneration(int n);
    void increaseEnergy(int n);

    // Getters
    Gene getGeneAt(size_t i) const;
    GeneManager getGenes() const;
    int getMaxGenes() const;
    const RobotStats getStats() const;
    double getAvgFitness() const;
    int getMutations() const;
    bool isMutant() const;
    bool isAlive() const;

    // Overloaded '+' Operator for reproducing
    Robot operator+(const Robot& other) const;

    // Overloaded '=' Operator
    Robot& operator=(const Robot& old);
};