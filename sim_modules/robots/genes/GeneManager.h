/*============================================================================*
File: GeneManager.h
Desc: Manages the set of genes for each Robot
*============================================================================*/

#pragma once

#include <vector>

#include "Gene.h"

using namespace std;

/*    GeneManager Class    */
// Manages the set of genes for each Robot
class GeneManager {

    vector<Gene> genes;     // Stores all 16 genes

public:
    GeneManager();
    GeneManager(const GeneManager& copy);

    // Randomly generate all genes
    void generateGenes();

    // Compare sensor data to stored genes and return corresponding action
    char compareGenes(array<char, 4> data);

    // Setters
    void setGeneAt(size_t i, const Gene& newGene);
    void setGenes(const vector<Gene>& newGenes);
    void addGene(const Gene& newGene);
    void clear();

    // Getters
    vector<Gene> getAllGenes() const;
    Gene getGeneAt(size_t i) const;
    int getMaxGenes() const;

    GeneManager& operator=(const GeneManager& copy);
};
