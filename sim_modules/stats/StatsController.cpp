/*============================================================================*
File: StatsController.cpp
Desc: Implementation of the StatsController class
*============================================================================*/

#include "StatsController.h"

StatController::StatController() : classLog(FUNC) {}

void StatController::printFinalGraph() {
    track.plotStats();
    view.printResults(track);
}

void StatController::printGenerationalStats() const { view.printGenerationalStats(track); }

void StatController::addStats(const RobotStats& newStats) { track.addStats(newStats); }

StatController& StatController::operator=(const StatController& copy) {
    track = copy.track;
    return *this;
}