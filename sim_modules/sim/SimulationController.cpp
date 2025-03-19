/*============================================================================*
File: SimulationController.cpp
Desc: Implementation file for the SimulationController class.
*============================================================================*/

#include "SimulationController.h"

// Constructors/Destructors
SimulationController::SimulationController() : classLog(FUNC) {}
SimulationController::SimulationController(const SimulationController& copy) { *this = copy; }
SimulationController::~SimulationController() { bots.clear(); }

void SimulationController::beginSim() {
    Logger log(FUNC);

    Timer simTimer; // Time the simulation runtime

    // Cycle through each generation
    simCycle();

    // Delete remaining bots & add their stats
    purgeRemainingBots();

    // Print Stats
    stats.printFinalGraph();

    // Print total simulation time
    simTimer.stopTimer();
    cout << COLOR("Total Time: ", CYAN)
        << COLOR(simTimer.calculateTime(), YELLOW)
        << COLOR(" seconds\n\n", BLACK, BRIGHT);
}

// Cycles through each generation, performing the core events
void SimulationController::simCycle() {
    Logger log(FUNC);

    while (currGen < MAX_GENERATIONS) {
        if (REPORT_INTERVAL > 0) timer.startTimer();

        currGen++;
        newGeneration();
        generationCycle();

        // Progress report
        if (REPORT_INTERVAL < 1) continue;
        if (currGen % int(REPORT_INTERVAL) == 0) {
            timer.stopTimer();
            progressReport();
        }
    }
}

// Cycles through each bot in the current generation
void SimulationController::generationCycle() {
    Logger log(FUNC);
    log.logTime("Generation " + to_string(currGen) + " Started");

    for (size_t i = 0; i < bots.size(); ++i)
        bots[i]->botCycle();
}

// Creates a new generation of bots
void SimulationController::newGeneration() {

    // Initialize the first generation
    if (currGen == 1) {
        initializePopulation();
        return;
    }

    // Prevent the final generation from breeding
    if (currGen > MAX_GENERATIONS) return;

    // Sort, then purge + breed based on performance
    sortBots();
    purgePopulation();
    breedPopulation();
}

// Thanos-Snap the bottom 50% of performers
void SimulationController::purgePopulation() {
    Logger log(FUNC);
    size_t midway = MAX_POPULATION / 2;

    while (bots.size() > midway && !bots.empty()) {
        addBotStats(bots.back());   // Log bot's stats before deletion
        bots.pop_back();
    }
}

// Breed the top 50% of performers
void SimulationController::breedPopulation() {
    Logger log(FUNC);

    size_t midway = MAX_POPULATION / 2;
    size_t i = 0;
    while (i < midway && bots.size() < size_t(MAX_POPULATION - 1)) {

        shared_ptr<RobotController> child1 = make_shared
            <RobotController>(*(bots.at(i)) + *(bots.at(i + 1)));
        child1->bot.setGeneration(currGen);
        bots.push_back(child1);

        shared_ptr<RobotController> child2 = make_shared
            <RobotController>(*(bots.at(i)) + *(bots.at(i + 1)));
        child2->bot.setGeneration(currGen);
        bots.push_back(child2);

        i += 2;
    }
}

// Log a Robot's stats to the Stat-Tracker
void SimulationController::addBotStats(const shared_ptr<RobotController>& bot) {
    Logger log(FUNC, 5);
    RobotStats curStats = bot->bot.getStats();
    stats.addStats(curStats);
}

// Logs all the remaining Bots to the Stat-Tracker
void SimulationController::purgeRemainingBots() {
    Logger log(FUNC);
    while (!bots.empty()) {
        addBotStats(bots.back());
        bots.pop_back();
    }
}

// Sort Bots by average fitness
void SimulationController::sortBots() {
    Logger log(FUNC, 4);
    std::sort(bots.begin(), bots.end(),
        [](const shared_ptr<RobotController>& a,
            const shared_ptr<RobotController>& b) {
                return a->bot.getAvgFitness() > b->bot.getAvgFitness();
        });
}

void SimulationController::clearBots() { bots.clear(); }

// Initialize the first generation of bots with random genes
void SimulationController::initializePopulation() {
    Logger log(FUNC);

    for (size_t i = 0; i < MAX_POPULATION; ++i) {
        shared_ptr<RobotController> newBot = make_shared<RobotController>();
        newBot->bot.setGeneration(currGen);
        bots.push_back(newBot);
    }
}

// Updates the user on the simulation's progress
void SimulationController::progressReport() const {
    FormatText ft;

    // Clear previous progress report
    if (currGen > REPORT_INTERVAL) ft.clearLines(3);
    if (currGen > REPORT_INTERVAL * 3) ft.clearLines(1);

    const int WIDTH = 40;
    cout << "\n" << align("Simulation Progress:", 'C', WIDTH) << endl;
    cout << ft.statusBar(currGen, MAX_GENERATIONS, WIDTH) << endl;

    if (currGen > REPORT_INTERVAL * 2) {
        int calcFormula = int(round((MAX_GENERATIONS - currGen)));
        string calcTime = timer.calculateTime(calcFormula);
        cout << align(COLOR("Estimated Time: ", CYAN), 'R', WIDTH / 2 + 5)
            << COLOR(calcTime, YELLOW) << " s\n";
    }
}

int SimulationController::getCurrGen() const { return currGen; }

SimulationController& SimulationController::operator=(const SimulationController& copy) {
    if (this != &copy) {
        currGen = copy.currGen;
        bots = copy.bots;
        stats = copy.stats;
    }
    return *this;
}
