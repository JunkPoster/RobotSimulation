/*============================================================================*
File: MapEngine.cpp
Desc: Implementation of the MapEngine class
*============================================================================*/

#include "MapEngine.h"

MapEngine::MapEngine() : classLog(FUNC, 4), map(MAP_WIDTH, MAP_HEIGHT) { generateMap(); }

// Randomly place Robot and then the Batteries
void MapEngine::generateMap() {
    Logger log(FUNC, 6);

    map.wipeGrid(EMPTY);
    numBatteries = 0;

    // Place Robot
    int x = rand.getRand(0, MAP_WIDTH - 1);
    int y = rand.getRand(0, MAP_HEIGHT - 1);
    map.setCell(x, y, ROBOT);

    // Fill the map with batteries
    while (numBatteries < MAX_BATTERIES) {
        x = rand.getRand(0, MAP_WIDTH - 1);
        y = rand.getRand(0, MAP_HEIGHT - 1);

        if (map.getCell(x, y) != EMPTY) continue;   // Skip filled cells
        map.setCell(x, y, BATTERY);                 // Set cell as Battery
        numBatteries++;
    }
}

// Setters
void MapEngine::emptyCell(int x, int y) { map.setCell(x, y, EMPTY); }
void MapEngine::setCell(int x, int y, char v) { map.setCell(x, y, v); }
void MapEngine::decreaseBatteries() { numBatteries--; }

// Getters
int MapEngine::getMapWidth() const { return MAP_WIDTH; }
int MapEngine::getMapHeight() const { return MAP_HEIGHT; }
char MapEngine::getCell(int x, int y) const {
    if (x < 0 || x > MAP_WIDTH - 1) return WALL;
    if (y < 0 || y > MAP_HEIGHT - 1) return WALL;
    return map.getCell(x, y);
}

MapEngine& MapEngine::operator=(const MapEngine& old) {
    numBatteries = old.numBatteries;
    map = old.map;
    return *this;
}