/*============================================================================*
File: StatsEngine.h
Desc: This class manages the core functionality of all statistic-related
        functionality in the program. It stores the stats of each robot and
        generation, and it calculates and sorts the stats for display.
*============================================================================*/

#pragma once

#include <vector>
#include <algorithm>

#include "../../utilities/logger/Logger.h"
#include "../helpers/Grid.h"
#include "../../SimulationSettings.h"
#include "StatsGraphProperties.h"
#include "StatsRobots.h"
#include "StatsGenerations.h"

using namespace std;

/*    StatEngine Class    */
class StatEngine {

    Logger classLog;
    StatGraphProperties props;

    Grid<char> grid;
    vector<RobotStats> botStats;
    vector<GenerationStats> genStats;

public:
    StatEngine();

    GenerationStats getGenStats(size_t g) const;
    char getCell(int x, int y) const;

    void addStats(const RobotStats& newStats);
    void calculateStats();
    void plotStats();
    void sortBotStats();
    void sortGenStats();

    StatEngine& operator=(const StatEngine& copy);
};