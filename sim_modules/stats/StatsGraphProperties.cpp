/*============================================================================*
File: StatsGraphProperties.cpp
Desc: Implementation of the StatGraphProperties struct
*============================================================================*/

#include "StatsGraphProperties.h"

StatGraphProperties::StatGraphProperties() {
    adjustGraph();
}

void StatGraphProperties::adjustGraph() {
    if (MAX_GENERATIONS > MAX_GRID_WIDTH) gridWidth = MAX_GRID_WIDTH;
    else gridWidth = MAX_GENERATIONS;
}

bool StatGraphProperties::isInterval(int i) const {
    if (MAX_GENERATIONS <= MAX_GRID_WIDTH) return false;
    if (i == 0) return false;
    if (i == gridWidth - 1) return false;
    for (int j = 0; j < MAX_GENERATIONS; j += interval) {
        if (i == j) return false;
    }
    return true;
}

int StatGraphProperties::getWidth() const {
    return gridWidth;
}

int StatGraphProperties::getHeight() const {
    return MAX_GRID_HEIGHT;
}
