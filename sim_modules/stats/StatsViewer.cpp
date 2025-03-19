/*============================================================================*
File: StatsViewer.cpp
Desc: Implementation of the StatViewer struct
*============================================================================*/

#include "StatsViewer.h"

// Print a graph of the final results of the simulation
void StatViewer::printResults(const StatEngine& stats) const {
    Logger log(FUNC, 4);

    string str, yAxis;
    int maxHeight = props.getHeight();
    int maxWidth = props.getWidth();
    const int yLblWidth = 4;  // Width for the Y-axis labels

    printHeader("Average Fitness Scores Per-Generation:");
    cout << uiBar(OUTPUT_WIDTH, '=');

    for (int y = maxHeight - 1; y >= 0; y--) {

        // Assign Y-Axis labels (skipping over certain intervals)
        if (y % 2 != 0) yAxis = COLOR((y + 1) * GRAPH_HEIGHT_SCALE, CYAN);
        else yAxis = COLOR('-', BLACK, BRIGHT);

        // Y-Axis Labels (Left Side)
        cout << "\n" << VC << align((" " + yAxis), 'C', yLblWidth) << VC;

        // Print each generation's avg Fitness level as bars
        for (int x = 0; x < maxWidth; x++) {
            // Get and determine the Cell's contents
            char cell = stats.getCell(x, y);
            if (cell == EMPTY)
                str = COLOR("   ", WHITE);  // Empty
            if (((int)y % 2) != 0)
                str = COLOR("___", BLACK, BRIGHT, UNDERLINE);
            if (cell != EMPTY)
                str = COLOR("===", GREENBG, GREEN);

            cout << str << VC;
        }
        // Y-Axis Labels (Right Side)
        cout << " " << align(yAxis, 'L', yLblWidth - 1) << VC;
    }

    // Bottom UI Styling
    cout << endl << uiBar(OUTPUT_WIDTH, '=')
        << endl << uiBar(yLblWidth + 1, '=') << " ";

    // X-Axis Labels
    for (int i = 1; i <= maxWidth; i++) {
        int genNum = stats.getGenStats(i).generation;
        string genStr = to_string(genNum);

        // Truncate larger numbers
        if (genNum > 999) genStr = to_string(genNum * 0.001).substr(0, 3);

        cout << align(COLOR(genStr, YELLOW), 'C', 3) << " ";
    }

    // Bottom Border
    cout << uiBar(yLblWidth + 1, '=');
    cout << titleBar(COLOR("Generation #", YELLOW), '(', ')');
}

// Prints detailed stats regarding each Generation. A bit sloppy
void StatViewer::printGenerationalStats(const StatEngine& stats) const {
    Logger log(FUNC, 4);

    // Header
    cout << "__________________________________________________________\n";
    cout << "| Gen # | Avg Fitness | Avg Turns | Avg Rounds | Mutants |\n";
    cout << "|-------|-------------|-----------|------------|---------|\n";

    for (int i = 1; i <= props.getWidth(); ++i) {
        if (i > MAX_GENERATIONS) break;
        GenerationStats genStats = stats.getGenStats(i);
        cout << "|" << align(COLOR(genStats.generation, GREEN), 'C', 7)
            << "|" << align(COLOR(genStats.avgFitness, CYAN, BRIGHT), 'C', 13)
            << "|" << align(COLOR(genStats.avgTurns, CYAN), 'C', 11)
            << "|" << align(COLOR(genStats.avgRounds, BLUE, BRIGHT), 'C', 12)
            << "|" << align(COLOR(genStats.numMutants, RED, BRIGHT), 'C', 9)
            << "|\n";
    }
    cout << "|_______|_____________|___________|____________|_________|\n";
}

// Simple header for the UI
void StatViewer::printHeader(string s) const {
    cout << "\n" << align(COLOR(s, YELLOW, BOLD), 'C', OUTPUT_WIDTH)
        << "\n";
}

// Similar to printHeader, but with a border
string StatViewer::titleBar(const string& title, char lChar, char rChar) const {
    string plate = string(1, lChar) + " " + title + " " + string(1, rChar);
    return "\n" + align(plate, 'C', OUTPUT_WIDTH, '=') + "\n";
}

string StatViewer::uiBar(int l, char c) const { 
    return COLOR(string(l, c), uiColor); 
}