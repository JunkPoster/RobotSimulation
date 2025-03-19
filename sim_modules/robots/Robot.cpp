/*============================================================================*
File: Robot.cpp
Desc: Implementation of the Robot class
*============================================================================*/

#include "Robot.h"

Robot::Robot() : energy(STARTING_ENERGY) {}
Robot::Robot(const Robot& copy) { *this = copy; }

// Analyze map surroundings, compare with Genes, and return action.
char Robot::processSensorData(array<char, 4> area) {
    return genes.compareGenes(area);
}

// Determine if the Robot is still alive
void Robot::checkHealth() {
    if (energy > 0 || !alive) return;
    energy = 0;
    alive = false;
}

// Reset the Bot's base stats for a new round
void Robot::startRound() {
    energy = STARTING_ENERGY;
    alive = true;
    stats.rounds += 1;
}

// Setters
void Robot::increaseTurns() { if (alive) stats.turns++; }
void Robot::increaseMutations() { stats.mutations++; }
void Robot::expendEnergy(int n) { if (alive) energy -= n; }
void Robot::setGeneration(int n) { stats.generation = n; }
void Robot::increaseEnergy(int n) {
    if (!alive) return;
    energy += n;
    stats.fitness += FITNESS_INCREASE;
}

// Getters
Gene Robot::getGeneAt(size_t i) const { return genes.getGeneAt(i); }
GeneManager Robot::getGenes() const { return genes; }
int Robot::getMaxGenes() const { return GENES_PER_BOT; }

const RobotStats Robot::getStats() const { return stats; }
double Robot::getAvgFitness() const { return stats.getAvgFitness(); }
int Robot::getMutations() const { return stats.mutations; }
bool Robot::isMutant() const { return (stats.mutations > 0); }
bool Robot::isAlive() const { return alive; }

// Overloaded '+' Operator for reproducing
Robot Robot::operator+(const Robot& other) const {
    Robot child;

    for (int i = 0; i < GENES_PER_BOT; ++i) {
        // Combine parents' genes (random parent for each gene)
        Gene childGene;
        if (rand.getRand(0, 1) == 0) childGene = genes.getGeneAt(i);
        else childGene = other.genes.getGeneAt(i);

        // Gene Mutation (5% chance by default)
        int mutationCheck = rand.getRand(1, 100);
        if (mutationCheck <= GENE_MUTATION_CHANCE) {
            childGene = childGene.mutateGene();
            child.increaseMutations();
        }
        child.genes.setGeneAt(i, childGene);
    }
    return child;
}

// Overloaded '=' Operator
Robot& Robot::operator=(const Robot& old) {
    energy = old.energy;
    alive = old.alive;
    genes = old.genes;
    stats = old.stats;  // Stats are copied for the mutation count
    // No other stats are affected by this.
    return *this;
}