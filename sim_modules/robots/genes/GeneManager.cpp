/*============================================================================*
File: GeneManager.cpp
Desc: Implementation file for the GeneManager class
*============================================================================*/

#include "GeneManager.h"

GeneManager::GeneManager() { generateGenes(); }
GeneManager::GeneManager(const GeneManager& copy) { *this = copy; }

// Randomly generate all genes
void GeneManager::generateGenes() {
    genes.clear();
    for (int i = 0; i < GENES_PER_BOT; ++i) genes.push_back(Gene());
}

// Compare sensor data to stored genes and return corresponding action
char GeneManager::compareGenes(array<char, 4> data) {
    for (size_t i = 0; i < genes.size(); ++i) {
        Gene curGene = genes[i];
        if (curGene.getSensors() == data) {
            char action = curGene.getAction();
            if (action != RANDOM) return curGene.getAction();
            else return curGene.genRandAction();
        }
    }
    return genes.back().getAction();    // Return last gene's action 
}

// Setters
void GeneManager::setGeneAt(size_t i, const Gene& newGene) { genes.at(i) = newGene; }
void GeneManager::setGenes(const vector<Gene>& newGenes) { genes = newGenes; }
void GeneManager::addGene(const Gene& newGene) { genes.push_back(newGene); }
void GeneManager::clear() { genes.clear(); }

// Getters
vector<Gene> GeneManager::getAllGenes() const { return genes; }
Gene GeneManager::getGeneAt(size_t i) const { return genes[i]; }
int GeneManager::getMaxGenes() const { return GENES_PER_BOT; }

GeneManager& GeneManager::operator=(const GeneManager& copy) {
    genes = copy.genes;
    return *this;
}