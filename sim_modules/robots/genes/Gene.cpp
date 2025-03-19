/*============================================================================*
File: Gene.cpp
Desc: Implementation file for the Gene class.
*============================================================================*/

#include "Gene.h"

using namespace std;

// Constructors
Gene::Gene() { 
    generateGene(); 
}

Gene::Gene(const Gene& copy) { 
    *this = copy; 
}

// Getters
array<char, Gene::NUM_GENE_CODES> Gene::getGene() const { return gene; }
char Gene::getGeneAt(size_t i) const { return gene[i]; }
char Gene::getAction() const { return gene[size_t(NUM_GENE_CODES - 1)]; }

// Return random sensor or action
char Gene::genRandSensor() const { 
    return sensor[rand.getRand(0, NUM_SENSORS - 1)]; 
}

char Gene::genRandAction() const { 
    return action[rand.getRand(0, NUM_ACTIONS - 1)]; 
}

// Return all sensors (for map scanning)
array<char, Gene::NUM_SENSORS> Gene::getSensors() const {
    array<char, NUM_SENSORS> sensors = { EMPTY, EMPTY, EMPTY, EMPTY };
    for (size_t i = 0; i < NUM_SENSORS; ++i) sensors[i] = gene[i];
    return sensors;
}

// Mutate a random node for a gene
Gene Gene::mutateGene() const {
    // Create a copy
    Gene mutatedGene(*this);
    int pos = rand.getRand(0, NUM_GENE_CODES - 1);
    char mutatedCode = gene[pos];

    // Randomly replace one code (until it's different).
    while (mutatedCode == gene[pos]) {
        // If the mutation happens for a sensor
        if (pos < NUM_GENE_CODES - 1) mutatedCode = genRandSensor();
        // If the mutation happens for an action
        else mutatedCode = genRandAction();
    }
    mutatedGene.gene[pos] = mutatedCode;
    return mutatedGene;
}

// Randomly generate Gene's data
void Gene::generateGene() {
    for (int i = 0; i < NUM_GENE_CODES - 1; ++i) gene[i] = genRandSensor();
    gene[size_t(NUM_GENE_CODES - 1)] = genRandAction();
}

// For Gene comparisons (Component Testing)
bool Gene::operator==(const Gene& other) const {
    for (size_t i = 0; i < gene.size(); ++i) {
        if (gene[i] != other.gene[i]) return false;
    }
    return true;
}

Gene& Gene::operator=(const Gene& copy) {
    gene = copy.gene;
    return *this;
}