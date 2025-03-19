/*============================================================================*
File: ColorText.h
Desc: This file contains a struct that colorizes text using ANSI codes, and
      an enum that contains the ANSI codes for various colors and modifiers.
*============================================================================*/

#pragma once

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

/******************************
*    ColorText Class & Enum   *
******************************/
/*    ANSI Color Codes used in ColorText    */
enum Colors {
    // Foreground       
    BLACK = 30,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    WHITE = 37,

    // Background       
    BLACKBG = 40,
    REDBG = 41,
    GREENBG = 42,
    YELLOWBG = 43,
    BLUEBG = 44,
    MAGENTABG = 45,
    CYANBG = 46,
    WHITEBG = 47,

    // Modifiers
    BOLD = 1,
    BRIGHT = 1,
    UNDERLINE = 4,
    INVERSE = 7,

    RESET = 0
};


/*    ColorText Struct    */
// Colorizes 'val', returning it as a string.
struct ColorText {
    // Template allows for varying datatypes. Modifiers are optional.
    template<typename T>
    string colorize(const T& val, Colors colorCode,
        Colors modifier1 = RESET, Colors modifier2 = RESET)
    {
        ostringstream oss;
        oss << "\033["
            << ((modifier1 == RESET) ? "" : (to_string(modifier1) + ";"))
            << ((modifier2 == RESET) ? "" : (to_string(modifier2) + ";"))
            << colorCode << "m" << val << "\033[" << RESET << "m";
        return oss.str();
    }
};

// Global function (instead of using a Macro)
template<typename T>
string COLOR(const T& val, Colors clr, Colors mod1 = RESET,
    Colors mod2 = RESET) {
    return ColorText().colorize(val, clr, mod1, mod2);
}
