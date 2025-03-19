/*============================================================================*
File: StatsViewer.h
Desc: This struct is responsible for displaying the stats in a graphical 
        format. It uses the StatGraphProperties struct to determine the
        display properties.
*============================================================================*/

#pragma once

#include "StatsEngine.h"
#include "../../utilities/FormatText.h"

using namespace std;

/*    StatViewer Struct    */
 // This class is responsible for displaying the stats in a graphical format
struct StatViewer {

    StatGraphProperties props;

    // Max width for the UI elements
    const int OUTPUT_WIDTH = (props.getWidth() * 4) + 11;

    // Graph element styles
    Colors uiColor = WHITE;             // Color of UI most elements
    string BC = COLOR('=', uiColor);    // Border char
    string VC = COLOR('|', uiColor);    // Vertical char
    string HC = COLOR('-', uiColor);    // Horizontal char
    string JC = COLOR('+', uiColor);    // Joint char

    void printResults(const StatEngine& stats) const;
    void printGenerationalStats(const StatEngine& stats) const;
    void printHeader(std::string s) const;
    std::string titleBar(const std::string& title, char lChar, char rChar) const;
    std::string uiBar(int l, char c) const;
};