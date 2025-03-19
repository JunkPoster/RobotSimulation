/*============================================================================*
File: Gene.h
Desc: Contains the Gene struct, which houses an individual gene for a Robot.
*============================================================================*/

#pragma once

#include <array>

#include "../../../utilities/RandNo.h"
#include "../../../SimulationSettings.h"

using namespace std;

/*    Gene Struct    */
// Houses an individual gene for a Robot (4 sensors, 1 action)
struct Gene {

    RandNo rand;

    static const int NUM_GENE_CODES = 5;
    static const int NUM_ACTIONS = 5;
    static const int NUM_SENSORS = 4;
    char action[NUM_ACTIONS] = { UP, DOWN, LEFT, RIGHT, RANDOM };
    char sensor[NUM_SENSORS] = { EMPTY, WALL, BATTERY, IGNORE };

    array<char, NUM_GENE_CODES> gene;

    // Constructors
    Gene();
    Gene(const Gene& copy);

    // Getters
    array<char, NUM_GENE_CODES> getGene() const;
    char getGeneAt(size_t i) const;
    char getAction() const;

    // Return random sensor or action
    char genRandSensor() const;
    char genRandAction() const;

    // Return all sensors (for map scanning)
    array<char, NUM_SENSORS> getSensors() const;

    // Mutate a random node for a gene
    Gene mutateGene() const;

    // Randomly generate Gene's data
    void generateGene();

    // For Gene comparisons (Component Testing)
    bool operator==(const Gene& other) const;

    Gene& operator=(const Gene& copy);
};