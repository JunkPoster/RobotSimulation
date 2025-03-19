/*============================================================================*
File: RobotController.cpp
Desc: Implementation of the RobotController class
*============================================================================*/

#include "RobotController.h"

// Functions
RobotController::RobotController() : classLog(FUNC, 4) {}
RobotController::RobotController(const RobotController& copy) { *this = copy; }

// Main function that cycles through a bot's turn
void RobotController::botCycle() {
    Logger log(FUNC, 6);

    // Reset the Map & the Bot's stats to test it again
    bot.startRound();
    map.resetMap();

    // While the Bot is alive it will search for batteries until it dies.
    while (bot.isAlive()) {
        bot.expendEnergy(ENERGY_COST);

        // Map scans area, Bot processes sensor data, Map moves bot.
        char move = bot.processSensorData(map.scan());
        if (map.moveRobot(move)) {
            bot.increaseEnergy(BATTERY_BOOST);
        }
        // Only increase turns if the bot is still alive
        bot.checkHealth();
        if (bot.isAlive()) bot.increaseTurns();
    }
}

RobotController RobotController::operator+(const RobotController& other) const {
    RobotController child;
    child.bot = bot + other.bot;
    return child;
}

RobotController& RobotController::operator=(const RobotController& old) {
    map = old.map;
    bot = old.bot;
    return *this;
}