#include <iostream>
#include <string>
#include <sstream>
//#include <cctype>
//#include <cstring>
using namespace std;

/******************************
*    ColorText Class & Enum   *
******************************/
// Using enum Colors, this will colorize 'val', returning it as a string.
enum Colors {
    // Foreground
    RESET   = 0,
    BLACK   = 30,
    RED     = 31,
    GREEN   = 32,
    YELLOW  = 33,
    BLUE    = 34,
    MAGENTA = 35,
    CYAN    = 36,
    WHITE   = 37,
    // Background
    BLACKBG   = 40,
    REDBG     = 41,
    GREENBG   = 42,
    YELLOWBG  = 43,
    BLUEBG    = 44,
    MAGENTABG = 45,
    CYANBG    = 46,
    WHITEBG   = 47,
    // Modifiers
    BOLD      = 1,
    BRIGHT    = 1,
    UNDERLINE = 4,
    INVERSE   = 7
};

// Colorizes 'val', returning it as a string.
struct ColorText {
    template<typename T>    // Allows varying datatypes and optional modifiers
    string colorize(const T& val, Colors colorCode, 
                    Colors modifier1 = RESET, Colors modifier2 = RESET) {
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
string COLOR(const T& val, Colors color, 
                Colors modifier1 = RESET, Colors modifier2 = RESET) {
    return ColorText().colorize(val, color, modifier1, modifier2);
}




/* 
 * OLD DESIGN
 */
/*
//#define COLOR(val, color) ColorText().colorize(val, color)

struct ColorText {

    // Base coloring function (string)
    string colorize(string str, Colors color) {
        string newStr = "\033[";
        newStr += to_string(color) + "m" + str;
        newStr += "\033[0m"; // reset to default
        return newStr;
    }
    // Colorize Char
    string colorize(char chr, Colors color) {
        return colorize(string(1, chr), color);
    }
    // Colorize Integer Number
    string colorize(int num, Colors color) {
        return colorize(to_string(num), color);
    }
    // Colorize Real Number
    string colorize(double num, Colors color) {
        return colorize(to_string(num), color);
    }    
};
*/