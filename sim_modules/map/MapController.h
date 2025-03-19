/*============================================================================*
File: MapController.h
Desc: Controls all Map-Related functions and data.
*============================================================================*/

#pragma once

#include <array>

#include "MapEngine.h"

using namespace std;

/*    MapController Class    */
class MapController {

    MapEngine map;

    int curX, curY;         // Robot's current location
    int maxX, maxY;         // Max X/Y values for the map
    array<char, 4> area;    // Area around the robot

    // Safe Increment/Reductions to prevent out-of-bounds
    void incY();
    void decY();
    void incX();
    void decX();

public:
    MapController();

    array<char, 4> MapController::scan();   // Scans the area around the robot
    bool moveRobot(char d);                 // Moves the robot in a direction
    void consumeBatteryAt(int x, int y);    // Consumes battery at a location
    void updateRobotLocation();             // Updates the robot's location
    void resetMap();                        // Resets the map to its OG state

    MapController& operator=(const MapController& old);
};