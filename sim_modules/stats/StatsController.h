/*============================================================================*
File: StatsController.h
Desc: This manages the StatsEngine and StatViewer classes. It interfaces the
    StatViewer struct with the StatEngine class.
*============================================================================*/

#pragma once

#include "../../utilities/logger/Logger.h"
#include "StatsEngine.h"
#include "StatsViewer.h"

using namespace std;

/*    StatController Struct    */
// Interfaces the StatViewer struct with the StatEngine class
struct StatController {

    Logger classLog;
    StatEngine track;
    StatViewer view;

    StatController();

    void printFinalGraph();
    void printGenerationalStats() const;
    void addStats(const RobotStats& newStats);

    StatController& operator=(const StatController& copy);
};