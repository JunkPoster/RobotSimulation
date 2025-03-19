/*============================================================================*
File: StatsGenerations.h
Desc: Header file for the GenerationStats struct. This struct is responsible
        for storing the statistics of each generation.
*============================================================================*/

#pragma once

/*    GenerationStats Struct    */
struct GenerationStats {
    double avgFitness = 0.0;
    double avgTurns = 0.0;
    double avgRounds = 0.0;
    int numBots = 0;
    int generation = 0;
    int numMutants = 0;
};