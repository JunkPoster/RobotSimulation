/*============================================================================*
File: StatsEngine.cpp
Desc: Implementation of the StatsEngine class
*============================================================================*/

#include "StatsEngine.h"

StatEngine::StatEngine() : classLog(FUNC), grid(props.getWidth(), props.getHeight()) {}

// Store stats to botStats vector
void StatEngine::addStats(const RobotStats& newStats) { 
    botStats.push_back(newStats); 
}

GenerationStats StatEngine::getGenStats(size_t g) const { 
    return genStats[g - 1]; 
}

char StatEngine::getCell(int x, int y) const { 
    return grid.getCell(x, y); 
}

// Calculates the stats for each generation and stores them in 'genStats'
void StatEngine::calculateStats() {
    Logger log(FUNC);

    sortBotStats();

    // Go through each generation and calculate respective stats
    // If MAX_GENERATIONS is set to a high number, it will skip intervals
    // to match the output-width of the graph.
    for (int g = 0; g < MAX_GENERATIONS; g++) {
        if (props.isInterval(g)) continue;

        GenerationStats currGen;
        currGen.generation = g + 1;
        double sumFitness = 0;
        double sumTurns = 0;
        int sumRounds = 0;

        // Organize each bot by generation and calculate stats
        for (size_t i = 0; i < botStats.size(); ++i) {
            RobotStats currBot = botStats[i];
            if (currBot.generation == g + 1) {
                sumFitness += currBot.getAvgFitness();
                sumTurns += currBot.getAvgTurns();
                sumRounds += currBot.rounds;
                currGen.numBots++;
                if (currBot.mutations > 0) currGen.numMutants++;
            }
        }
        // Assign calculated stats to the current generation
        currGen.avgFitness = (sumFitness / currGen.numBots);
        currGen.avgTurns = sumTurns / currGen.numBots;
        currGen.avgRounds = sumRounds / (double)currGen.numBots;
        genStats.push_back(currGen);
    }
}

// Plot the calculated stats onto the graph
void StatEngine::plotStats() {
    Logger log(FUNC, 4);

    calculateStats();
    grid.wipeGrid(EMPTY);
    sortGenStats();

    // Plot each generation's stats onto the graph
    for (int g = 0; g < props.getWidth(); g++) {
        GenerationStats currGen = genStats[g];
        // Adjusts the height of the bar based on the height scale
        int avg = int(round(currGen.avgFitness) / GRAPH_HEIGHT_SCALE);
        for (int i = 0; i < avg - 1; i += 1) {
            grid.setCell(g, i, '=');
        }
    }
}

// Sort Robot stats by Generation number
void StatEngine::sortBotStats() {
    Logger log(FUNC, 4);
    std::sort(botStats.begin(), botStats.end(),
        [](const RobotStats& a, const RobotStats& b) {
            return a.generation < b.generation;
        });
}

// Sort Generational stats by Generation number
void StatEngine::sortGenStats() {
    Logger log(FUNC, 4);
    std::sort(genStats.begin(), genStats.end(),
        [](const GenerationStats& a, const GenerationStats& b) {
            return a.generation < b.generation;
        });
}

StatEngine& StatEngine::operator=(const StatEngine& copy) {
    grid = copy.grid;
    botStats = copy.botStats;
    genStats = copy.genStats;
    return *this;
}