/*============================================================================*
File: SimulationController.h
Desc: This class manages the houses the core simulation loop, and is 
    responsible for managing the RobotController and StatsController classes.
*============================================================================*/

#pragma once

#include <vector>
#include <memory>

#include "../../utilities/logger/Logger.h"
#include "../../utilities/Timer.h"
#include "../robots/RobotController.h"
#include "../stats/StatsController.h"

using namespace std;

/**************************
 *    SimulationController Class    *
 *************************/
 // Houses the core simulation loop
class SimulationController {

    Logger classLog;
    Timer timer;

    std::vector<std::shared_ptr<RobotController>> bots;

    int currGen = 0;
    const double REPORT_INTERVAL = MAX_GENERATIONS * (PROGRESS_UPDATE_RATE * 0.01);

public:
    StatController stats;
    SimulationController();
    SimulationController(const SimulationController& copy);
    ~SimulationController();

    void beginSim();

    void initializePopulation();
    void simCycle();
    void generationCycle();

    void newGeneration();
    void purgePopulation();
    void breedPopulation();
    void purgeRemainingBots();

    void addBotStats(const std::shared_ptr<RobotController>& bot);
    
    void sortBots();
    void clearBots();


    int getCurrGen() const;
    void progressReport() const;

    SimulationController& operator=(const SimulationController& copy);
};
