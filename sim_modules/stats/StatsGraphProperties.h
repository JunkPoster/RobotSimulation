/*============================================================================*
File: StatsGraphProperties.h
Desc: This file contains a struct that houses the properties for the StatGraph.
*============================================================================*/

#pragma once

#include "../../SimulationSettings.h"

/*    StatGraphProperties Struct    */
// This is my attempt at dynamically adjusting the graph's width and height
struct StatGraphProperties {

    // I advise against changing these values
    int MAX_GRID_HEIGHT = 20;
    int MAX_GRID_WIDTH = 25;
    int gridWidth;
    int interval = (MAX_GENERATIONS - 2) / (MAX_GRID_WIDTH - 2);

    StatGraphProperties();
    void adjustGraph();
    bool isInterval(int i) const;
    int getWidth() const;
    int getHeight() const;
};
