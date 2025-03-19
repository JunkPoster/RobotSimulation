/*============================================================================*
File: MapEngine.h
Desc: Holds the main logic of the map for each Robot instance. Uses my Grid
      class to manage the map.
*============================================================================*/

#pragma once

#include "../../utilities/logger/Logger.h"
#include "../../utilities/RandNo.h"
#include "../helpers/Grid.h"
#include "../../SimulationSettings.h"

using namespace std;

/*    MapEngine Class    */
 // Directly manages each Robot's instance of the map by using my Grid class
class MapEngine {

    Logger classLog;
    RandNo rand;
    Grid<char> map;

    // Map Properties
    static const int MAP_WIDTH = MAP_GRID_WIDTH;
    static const int MAP_HEIGHT = MAP_GRID_HEIGHT;
    static const int NUM_CELLS = MAP_WIDTH * MAP_HEIGHT;
    int MAX_BATTERIES // Using cmath.round to reduce potential precision loss
        = static_cast<int>(round(NUM_CELLS * (STARTING_BATTERY_RATE * 0.01)));

    int numBatteries = 0;

public:
    MapEngine();

    // Randomly place Robot and then the Batteries
    void generateMap();

    // Setters
    void emptyCell(int x, int y);
    void setCell(int x, int y, char v);
    void decreaseBatteries();

    // Getters
    int getMapWidth() const;
    int getMapHeight() const;
    char getCell(int x, int y) const;

    MapEngine& operator=(const MapEngine& old);
};