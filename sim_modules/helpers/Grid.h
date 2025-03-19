/*============================================================================*
File: Grid.h
Desc: Creates and manages a Grid object by creating a Vector of Vectors.
NOTE: I did a little fiddling to handle the grid on an X/Y basis rather than
        a ROW/COL basis.
*============================================================================*/

#pragma once

#include <iostream>
#include <vector>

#include "GridExceptions.h"
#include "../../utilities/logger/Logger.h"

using namespace std;

/*    Grid Class    */
// This will create a grid based on the passed WxH. All methods used to access
// the grid are done in X,Y format instead of ROW, COL.
template <typename T>
class Grid {

    Logger classLog;

    int MAX_WIDTH;
    int MAX_HEIGHT;
    int NUM_CELLS;

    vector<vector<T>> grid;

public:
    Grid(int maxW = 10, int maxH = 10)
        : classLog(FUNC, 3), MAX_WIDTH(maxW), MAX_HEIGHT(maxH),
        NUM_CELLS(MAX_WIDTH* MAX_HEIGHT),
        grid(MAX_HEIGHT, vector<T>(MAX_WIDTH)) {
    }

    Grid(const Grid& copy) { *this = copy; }

    void wipeGrid(T empty) {
        for (auto& row : grid) {
            fill(row.begin(), row.end(), empty);
        }
    }

    // I did some trickery to control the grid via X/Y axis, instead of ROW/COL
    void setCell(int x, int y, const T& v) {

        // Handle overflow
        if (x < 0) x = 0;
        else if (x >= MAX_WIDTH) x = MAX_WIDTH - 1;
        if (y < 0) y = 0;
        else if (y >= MAX_HEIGHT) y = MAX_HEIGHT - 1;

        y = (MAX_HEIGHT - 1) - y;
        grid[y][x] = v;
    }

    T getCell(int x, int y) const {
        if (x < 0 || x >= MAX_WIDTH || y < 0 || y >= MAX_HEIGHT) {
            throw grid_out_of_bounds("setCell(X: " + to_string(x)
                + ", Y: " + to_string(y) + ")");
        }
        y = (MAX_HEIGHT - 1) - y;
        return grid[y][x];
    }

    Grid& operator=(const Grid& old) {
        if (this != &old) {
            MAX_WIDTH = old.MAX_WIDTH;
            MAX_HEIGHT = old.MAX_HEIGHT;
            NUM_CELLS = old.NUM_CELLS;
            grid = old.grid;
        }
        return *this;
    }
};