/*============================================================================*
File: MapController.cpp
Desc: This file contains the implementation of the MapController class.
*============================================================================*/

#include "MapController.h"

void MapController::incY() { curY = min(curY + 1, maxY); }
void MapController::decY() { curY = max(curY - 1, 0); }
void MapController::incX() { curX = min(curX + 1, maxX); }
void MapController::decX() { curX = max(curX - 1, 0); }

MapController::MapController() {
    maxX = map.getMapWidth() - 1;
    maxY = map.getMapHeight() - 1;
    updateRobotLocation();
}

// Move Robot from its current position, depending on its decision
bool MapController::moveRobot(char d) {
    // Empty current cell
    map.setCell(curX, curY, EMPTY);

    // Adjust coords appropriately
    if (d == UP) incY();
    else if (d == DOWN) decY();
    else if (d == LEFT) decX();
    else if (d == RIGHT) incX();

    // If battery is present, consume it
    bool batteryConsumed = false;
    if (map.getCell(curX, curY) == BATTERY) {
        consumeBatteryAt(curX, curY);
        batteryConsumed = true;
    }
    map.setCell(curX, curY, ROBOT);
    return batteryConsumed;
}

// Scan nearby area an return char array
array<char, 4> MapController::scan() {
    area[0] = map.getCell(curX, curY + 1);  // Up
    area[1] = map.getCell(curX, curY - 1);  // Down
    area[2] = map.getCell(curX - 1, curY);  // Left
    area[3] = map.getCell(curX + 1, curY);  // Right
    return area;
}

void MapController::consumeBatteryAt(int x, int y) {
    map.emptyCell(x, y);
    map.decreaseBatteries();
}

void MapController::updateRobotLocation() {
    for (int y = map.getMapHeight() - 1; y >= 0; --y) {
        for (int x = 0; x < map.getMapWidth(); ++x) {
            if (map.getCell(x, y) == ROBOT) {
                curX = x;
                curY = y;
                return;
            }
        }
    }
}

void MapController::resetMap() {
    map.generateMap();
    updateRobotLocation();
}

MapController& MapController::operator=(const MapController& old) {
    map = old.map;
    curX = old.curX;
    curY = old.curY;
    area = old.area;

    return *this;
}