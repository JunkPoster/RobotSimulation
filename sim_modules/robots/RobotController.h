/*============================================================================*
File: RobotController.h
Desc: Manages the Robot and Map classes. This class is responsible for
    interfacing the Robot with the Map.
*============================================================================*/

#pragma once

#include "Robot.h"
#include "../map/MapController.h"

using namespace std;

/*===========================<BOT/MAP INTERFACE>=============================*/
/*    RobotController Class    */
// Interfaces Robots with Maps
struct RobotController {

    Logger classLog;
    MapController map;
    Robot bot;

    static const int BATTERY_BOOST = 5;    // Energy gained from batteries
    static const int ENERGY_COST = 1;      // Cost of moving

    // Functions
    RobotController();
    RobotController(const RobotController& copy);
    void botCycle();

    RobotController operator+(const RobotController& other) const;
    RobotController& operator=(const RobotController& old);
};