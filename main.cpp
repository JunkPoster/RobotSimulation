/*============================================================================*
FinalProj.cpp
Ian Featherston
Professor Fowler, CISP400
05/07/2024 - 05/16/2024

Through countless hours and trial-and-error, I'm finally happy with this
program. I had a fairly large 'SimInspector' class that supplmented the main
menu and detailed the behind-the-scenes operations for the main mechanics
like Breeding, Purging, Navigating, Scanning, etc.; however, I decided to
remove it because it was too verbose and didn't add much to the actual
program. Since it was primarily designed for debugging, it was pretty
cluttered and difficult to follow.

I've tried my best to steer clear of using Macros, but I used one to
supplement the Logger class to pass '__PRETTY_FUNCTION__' as 'FUNC'. I've also
used a few global functions as helpers to various classes in place of macros,
but I've tried to keep them to a minimum.

NOTE: I originally used raw pointers to store the population of Robot objects.
When checking all Logging activity I noticed that the vast majority of
classes/structs were NOT being destroyed, so I switched to using a vector of
shared_ptrs to avoid memory issues. This actually resulted in a small decrease
in runtime performance, but it *should* be more stable.

*============================================================================*/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>
#include <cstring>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <vector>
#include <array>
#include <memory>

using namespace std;
using namespace chrono;

/*==============================<GLOBAL SETTINGS>============================*/

// Which priority lvl to include up-to (Higher = more logging, -1 = OFF)
// You can read more about each Logging level in the Logger class.
/* NOTE: Logging was kept fairly low-profile for this program, and it will
    STILL produce tens-of-thousands of lines in the logfile.
    !! Lower Logging levels will have better performance. */
const int LOGGING_LEVEL = 2;

/****************************
*    SIMULATION SETTINGS    *
****************************/
// Globally alter various settings for the simulation
enum SIMULATION_SETTINGS {

    MAX_GENERATIONS = 2000,     // # of generations to simulate 
    MAX_POPULATION = 200,       // Forced Population size

    STARTING_BATTERY_RATE = 40, // % of batteries at start
    FITNESS_INCREASE = 5,       // Fitness increase
    STARTING_ENERGY = 5,        // Starting energy for each bot
    GENES_PER_BOT = 16,         // How many genese each bot has
    GENE_MUTATION_CHANCE = 5,   // Chance of gene mutation

    GRAPH_HEIGHT_SCALE = 4,     // Changes height ratio. Lower = taller bars
    PROGRESS_UPDATE_RATE = 5    // Progress-update Frequency (0 to disable)
};

/*===========================<CUSTOM EXCEPTIONS>=============================*/
// This exception is thrown when trying to access an invalid index in Grid().
struct grid_out_of_bounds : public std::exception {

    grid_out_of_bounds(const string& arg) { errorMsg(arg); }
    void errorMsg(const string& arg) {
        cout << "\n[GRID] ERROR: Invalid Index Requested! (" + arg + ")\n";
    }
};
/*===========================<CLASS DEFINITIONS>=============================*/

        /*===================<START OF UTILITIES>===================*/
/*    Logger Classes    */
// This fix is for VisualStudio, as it doesn't use __PRETTY_FUNCTION__
#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
#endif 

// Allows the passing of a Function's name as 'FUNC' (This is the only Macro)
#define FUNC __PRETTY_FUNCTION__

// Static instance to reduce file IO activity
class LoggerFileHandler {

    const string FILENAME = "log.txt";
    ofstream file;

public:
    LoggerFileHandler() { file.open(FILENAME, ios_base::out); }
    ~LoggerFileHandler() { if (file.is_open()) file.close(); }

    void startFile() { if (!file.is_open()) file.open(FILENAME, ios_base::out); }
    void appendFile() { if (!file.is_open()) file.open(FILENAME, ios_base::app); }
    void closeFile() { if (file.is_open()) file.close(); }
    void writeToLogFile(const string& msg) { file << msg; }

    LoggerFileHandler(const LoggerFileHandler&) = delete;
    LoggerFileHandler& operator=(const LoggerFileHandler&) = delete;
};

// Global File-Handler Instance to ensure only one file is created
LoggerFileHandler logfileHandler;

// I would still very much like to break this logger class into smaller
// pieces, but I'm happy with it for now.
class Logger {

    // Define amount of logging activity. Each number includes those before it.
    const int INCLUSION_LVL = LOGGING_LEVEL;
    /* 0  = Standalone Functions
    *  1  = Class Constructors/Destructors
    *  2  = Core Member Functions
    *  3  = Utilities
    *  4  = Debugging
    *  5  = Common Functions
    *  6+ = Extreme Logging */

    int eventPriority = 0;  // Sets Priority level of current event
    string eventType;       // Standalone, Class, Member, etc.
    string eventAction;     // Specifies event actions (Called, Ended, etc.)

    string funcName;        // Function name automatically passed using Macros
    char stamp[25];         // Contains timestamps

    time_t curTime = time(0);
    high_resolution_clock::time_point start, stop;

    static bool startOfProgram;     // Performs Start-Of-File formatting
    static bool loggingEnabled;     // Can be used to toggle Logging
    static string baseFunction;     // Starter Function's name for EOF logging

    // Start a new file with unique Header
    void startNewFile() {

        // Format file header
        strftime(stamp, sizeof(stamp), "%A, %m/%d/%Y", localtime(&curTime));
        string str = stamp;
        str += string((31 - str.size()), ' ') + "|\n"
            + timeStamp() + "PROGRAM STARTED |\n"
            + string(32, '=') + "\n| -> = Function Called   "
            + "|\n| <- = Function Returned |\n"
            + string(26, '=')
            + "\nInclusion Level: " + to_string(INCLUSION_LVL)
            + "\n\n[HH:MM:SS.ms ]\n";

        // Assign Static Variables
        baseFunction = funcName;
        startOfProgram = false;

        writeToFile(str);
    }

    // Applies End-of-File Formatting
    void endFile() {
        string endMsg = "\n" + string(35, '=') + "\n"
            + timeStamp() + "PROGRAM ENDED ";
        writeToFile(endMsg);
    }

    // Assigns the Action based on type (Called, Constructed, Returned, etc.)
    void formatFuncAction(const string& str = "") {
        // Adjust funcAction if endLog() called, then return
        if (str == "!end") {
            if (eventType == "CLASS") eventAction = "Destroyed";
            else eventAction = "Returned";
            return;
        }

        eventAction = "Called";      // Default action
        if (eventType == "CLASS") eventAction = "Constructed";
    }

    void formatEventPriority() {
        if (eventPriority != 0) return;
        if (eventType == "STANDALONE") eventPriority = 0;
        if (eventType == "CLASS") eventPriority = 1;
        if (eventType == "MEMBER") eventPriority = 2;
    }

    // Strip return type and parenthesis from __PRETTY_FUNCTION__
    void formatFuncName(const string& rawFuncName) {

        string str = rawFuncName;

        // Remove parenthesis and everything after
        size_t pos = str.find("(");
        if (pos != string::npos) str = str.substr(0, pos);

        // Remove return type (if it exists).
        pos = str.find(" ");
        if (pos != string::npos) str = str.substr(pos + 1);

        // If compiler is VisualStudio, adjust for class functions
        /* DEBUG: Remove this before submitting. If you're reading this...
            then I forgot. */
#ifdef _MSC_VER 
        pos = str.find(" ");
        if (pos != string::npos) str = str.substr(pos + 1);
#endif

        // Find Class name (if it exists).
        string prefix, suffix;
        pos = str.find(":");

        // Check if non-class function. Else, check if constructor or function
        if (pos == string::npos) {
            eventType = "STANDALONE";
            funcName = str;
        }
        else {
            prefix = str.substr(0, pos);
            suffix = str.substr(pos + 2);
            if (prefix == suffix) {
                funcName = prefix;
                eventType = "CLASS";
            }
            else {
                funcName = "  " + str;  // Apply minor spacing
                eventType = "MEMBER";
            }
        }
    }

    // Convert and return exact timestamp as string
    string timeStamp() {
        auto now = system_clock::now();
        auto now_2 = system_clock::to_time_t(now);
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        tm tm = *localtime(&now_2);
        strftime(stamp, sizeof(stamp), "[%H:%M:%S.", &tm);
        snprintf(stamp + strlen(stamp), sizeof(stamp) - strlen(stamp),
            "%03d] ", static_cast<int>(ms.count()));
        return stamp;
    }

public:
    // Constructors
    Logger() : curTime(time(0)) { timeStamp(); }
    // An integer can be passed to manually set the priority
    Logger(string func, int prio = 0, string msg = "") : eventPriority(prio) {
        startLog(func, msg);
    }
    ~Logger() { if (funcName != "") endLog(); }

    void writeToFile(const string& logMsg) const {
        if (INCLUSION_LVL < eventPriority) return;
        logfileHandler.writeToLogFile(logMsg);
    }

    // Logs the Start of a function
    void startLog(const string& name, const string& msg = "") {

        // Skip if logging is disabled or below the priority level
        if (INCLUSION_LVL < eventPriority) return;

        start = high_resolution_clock::now();

        // Format the current function's name, type, & action
        formatFuncName(name);
        formatFuncAction();
        if (eventPriority > 0) formatEventPriority();

        // Create new Logger file at the beginning of each run
        if (startOfProgram) startNewFile();

        // Create logMsg, adjusting the spacing based on Priority
        string logMsg = timeStamp() + "-> | "
            + string(eventPriority, ' ')
            + funcName + "(" + msg + ") "
            + eventAction + "\n";

        if (loggingEnabled) writeToFile(logMsg);
    }

    // Called to mark the END of a function
    void endLog(const string& msg = "") {
        stop = high_resolution_clock::now();
        auto elapsed = duration_cast<microseconds>(stop - start).count();
        string totalTime = " [" + to_string(elapsed) + " ms]\n";

        // "!end" changes the funcAction accordingly
        formatFuncAction("!end");

        // Create logMsg, adjusting the spacing based on Priority
        string logMsg = timeStamp() + "<- | "
            + string(eventPriority, ' ')
            + funcName + "(" + msg + ") "
            + eventAction + totalTime;

        if (loggingEnabled) writeToFile(logMsg);
        if (funcName == baseFunction) endFile();    // EOF message
    }


    // Timestamps a custom message (Allows for specifying priority)
    void logTime(const string& msg) { logTime(0, msg); }
    void logTime(int prio, const string& msg) {
        eventPriority = prio;
        writeToFile(timeStamp() + "[] | " + string(prio, ' ') + msg + "\n");
    }

    // Logs PASS/FAIL status for a ComponentTest().
    void logTestStatus(const string& testName, bool testPassed) const {
        string logMsg = string(testPassed ? "[PASSED] " : "[FAILED] ")
            + testName + " Testing Finished.\n\n";
        writeToFile(logMsg);
    }

    // Logs specific location of Component failures & error message
    void logErrorMsg(const string& testName, const string& errorMsg = "") {
        string logMsg = timeStamp() + "!!!| Failure Happened in "
            + testName + "\n" + string(18, ' ') + "| ";
        if (errorMsg != "") logMsg += "-- " + errorMsg + "\n";
        else logMsg += "-- No Error Message Provided.\n";

        writeToFile(logMsg);
    }

    void setLoggingStatus(bool flag) { loggingEnabled = flag; }
};

// Initialize static variables
bool Logger::loggingEnabled = true;
bool Logger::startOfProgram = true;
string Logger::baseFunction = "";

/*===================<TEXT FORMATTING SECTION>===================*/

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

/*    AlignText Class    */
struct AlignText {

    // Function to align text Left, Right, or Center
    // W = max-width that the string can be, L = length of the string
    string alignText(string str, char align, int W, char padChar = ' ') const {

        // Calculate padding based on alignment
        // C/R/L = Center/Right/Left-Aligned, respectively
        int L = static_cast<int>(visibleLength(str));
        int padLeft = 0, padRight = 0;

        switch (align) {
        case 'C':
            // Formula to center text by padding both sides.
            padLeft = padRight = (W - L) / 2 + ((W - L) % 2 != 0);

            // Adjust padding when mixing Odds/Evens.
            if (L % 2 == 0 && W % 2 != 0) padLeft -= 1;
            if (L % 2 != 0 && W % 2 == 0) padLeft -= 1;
            break;
        case 'R':
            padLeft = W - L;
            break;
        case 'L':
            padRight = W - L;
            break;
        }
        // Return string with appropriate padding
        return string(padLeft, padChar) + str + string(padRight, padChar);
    }

    // Overloaded to cast 'width' to int for alignText()
    string alignText(string str, char align, size_t W, char padChar = ' ')
        const {
        int intWidth = static_cast<int>(W);
        return AlignText().alignText(str, align, intWidth, padChar);
    }

    // Helper Function to adjust padding for colored text
    size_t visibleLength(const string& str) const {
        size_t length = 0;
        size_t i = 0;

        // Cycle through str, invisible vs. visible chars
        while (i < str.length()) {
            if (str[i] == '\033') {     // ANSI Code starts.
                while (i < str.length() && str[i] != 'm') ++i;
                ++i;
            }
            else {    // Normal characters increase length
                ++length;
                ++i;
            }
        }
        return length;
    }
};

/*    WrapText Class    */
// Child class to AlignText, which wraps text at a specified width.
struct WrapText : public AlignText {

    size_t INDENT_AMT;    // Amount of Indentation
    size_t TAB_SIZE = 4;  // Size of a Tab
    bool INDENT;          // Adjusts for Indentation behavior

    // Constructor to initialize indent behavior.
    WrapText(size_t indentAmt = 0) : INDENT_AMT(indentAmt) {
        if (INDENT_AMT > 0) {
            INDENT = true;
            INDENT_AMT = indentAmt - 1;
        }
        else {
            INDENT = false;
            INDENT_AMT = 0;
        }
    }

    // Overloaded to cast 'width' to size_t for wrapText()
    string wrapText(string str, char align, int tgtWidth, string fix = "\n")
        const {
        size_t width = static_cast<size_t>(tgtWidth);
        string newStr = wrapText(str, align, width, fix);
        return newStr;
    }

    // This word-wraps text at a specified width. Uses align().
    string wrapText(string str, char align, size_t width, string fix = "\n")
        const {

        // Adjust for '\n' and '\t' characters
        size_t newlines = countNewLineChars(str);
        replaceTabChars(str);

        // Variables for Word-Wrapping functionality
        size_t wraps;                       // Number of lines needed
        size_t length = visibleLength(str); // Counts visible characters
        string tempStr = str;               // Substringing of 'str'
        vector<string> cutStr;              // Stores substrings of 'tempStr'

        // Calculate 'wraps', which is the number of lines needed.
        if (newlines > 0) wraps = length / (width - 2) + newlines + 5;
        else wraps = length / (width - 2) + 5;

        // If the string is shorter than the width, return it as-is.
        if (length < width && newlines == 0) return alignText(str, align, width);

        // Divide string near width, OR at '\n'.
        for (size_t i = 0; i < wraps; i++) {

            // Indenting Behavior
            if (!INDENT) { if (tempStr[0] == ' ') tempStr.replace(0, 1, ""); }
            else if (i != 0) tempStr.insert(0, string(INDENT_AMT, ' '));

            // Create substrings at each newline character.
            if (newlines > 0) {
                string shortStr = tempStr.substr(0, width - 2);
                size_t newLinePos = shortStr.find('\n', 1);
                if (newLinePos != string::npos) {
                    string newLineStr = tempStr.replace(newLinePos, 1, " ");
                    cutStr.push_back(newLineStr.substr(0, newLinePos));
                    tempStr.replace(0, newLinePos, "");
                    continue;
                }
            }

            // If it's the last line, avoid substringing.
            if (visibleLength(tempStr) <= width - 2) {
                cutStr.push_back(tempStr);
                break;
            }

            // Seek spaces near end-of-line.
            // MODIFIED TO ADJUST FOR HIDDEN ANSI ESCAPE CHARACTERS
            size_t diff = width - (visibleLength(tempStr.substr(0, width - 2)));
            size_t adj = width + diff - 2;
            size_t pos = tempStr.rfind(' ', (adj - 2));
            if (pos <= tempStr.length()) {
                cutStr.push_back(tempStr.substr(0, pos));
                tempStr.replace(0, pos, "");
            }
            // If no space exists, hyphenate to newline.
            else if (pos == string::npos) {
                cutStr.push_back(tempStr.substr(0, width - 3) + "-");
                tempStr.replace(0, width - 3, "");
            }
        }

        // Combine each element of cutStr[]. Adjust for newline characters.
        string finalStr = "";
        for (size_t i = 0; i < cutStr.size(); i++) {
            if (!cutStr[i].empty()) {
                finalStr += alignText(cutStr[i], align, width);

                // Apply the optional 'fix' parameter (default = '\n')
                if (i < wraps - 1) finalStr += fix;
            }
        }
        return finalStr;
    }

    // Count and return the number of '\n' characters in a string.
    // Also replaces "\\n" with '\n' for proper formatting.
    size_t countNewLineChars(string& str) const {
        size_t numNewLines = 0;
        size_t foundStrNL = string::npos;
        size_t foundCharNL = string::npos;
        while (true) {
            foundStrNL = str.find("\\n", foundStrNL + 1);
            foundCharNL = str.find('\n', foundCharNL + 1);
            if (foundStrNL != string::npos) {
                str.replace(foundStrNL, 2, "\n");
                numNewLines++;
            }
            else if (foundCharNL != string::npos) numNewLines++;
            else break;
        }
        return numNewLines;
    }

    // Replace '\t' characters with spaces using TAB_SIZE.
    void replaceTabChars(string& str) const {
        size_t foundTab = string::npos;
        while (true) {
            foundTab = str.find('\t', foundTab + 1);
            if (foundTab != string::npos) {
                str.replace(foundTab, 2, "");
                str.insert(foundTab, TAB_SIZE, ' ');
            }
            else break;
        }
    }
};

/*    StatusBar Struct    */
// Prints a colorized status/progress bar based on the numbers passed
// I could definitely modularize this more, but I'm happy with it for now.
struct StatusBar {

    string statusBar(int cur, int max, int numBars) const {

        string statusBar;
        string bars = "";

        // Define Colors for each section 
        // I should be able to modularize this fairly easily
        const int NUM_SECTIONS = 3;
        Colors colors[NUM_SECTIONS] = { REDBG, YELLOWBG, GREENBG };
        int SECTION = max / NUM_SECTIONS;

        // Standardize values
        double num = (double(cur) / max) * 100;
        int lvl = static_cast<int>(round((num * .10) * (numBars * 0.10)));
        max = max * 1;

        // Determine color based on percentage
        Colors barColor = colors[NUM_SECTIONS - 1];
        Colors mod1 = BRIGHT;

        // Sub-section currently has no effect, since we're using Background 
        // colors
        for (int i = 0; i < NUM_SECTIONS; i++) {
            if (cur < SECTION * (i + 1)) {
                barColor = colors[i];
                // Determine sub-section (15% higher than current = BRIGHT)
                if (cur + (max / 100 * 15) < SECTION * (i + 1)) mod1 = RESET;
                break;
            }
        }

        // Final formatting
        bars = ColorText().colorize(string(lvl, ' '), barColor, mod1);
        bars = AlignText().alignText(bars, 'L', numBars);
        statusBar = "[" + bars + "]";
        return statusBar;
    }
};

// Allows the control of all Text-Altering classes in one object
struct FormatText : public ColorText, public WrapText, public StatusBar {

    FormatText(int indentAmt = 0) : WrapText(indentAmt) {
        Logger().logTime(3, "FormatText Object Created.");
    }

    // Clears Entire Screen
    void clearScreen() const { cout << "\033[2J\033[1;1H"; }

    // Clears 'n' lines from output
    void clearLines(int n) const {
        for (int i = 0; i < n; i++) cout << "\033[A\033[K";
    }
};

// Global function (instead of using a Macro)
string align(string str, char align, int W, char padChar = ' ') {
    return FormatText().alignText(str, align, W, padChar);
}

// Global function (instead of using a Macro)
template<typename T>
string COLOR(const T& val, Colors clr, Colors mod1 = RESET,
    Colors mod2 = RESET) {
    return ColorText().colorize(val, clr, mod1, mod2);
}

/*===================<OTHER UTILITIES>===================*/

/*    Date Class    */
// Stores the System time into strings. Returns date in MM/DD/YYYY format
class Date {

    Logger classLog;
    time_t now;
    int m, d, y;

public:
    Date() : classLog(FUNC, 3) { setDateVariables(); }
    ~Date() {}

    // Convert current system time to usable data (strings)
    void setDateVariables() {
        now = time(0);
        char mon[6], day[6], year[10];
        strftime(mon, 6, "%m", localtime(&now));
        strftime(day, 6, "%d", localtime(&now));
        strftime(year, 10, "%Y", localtime(&now));

        // Assign to mon/day/year variables
        m = atoi(mon), d = atoi(day), y = atoi(year);
    }

    // Return current date in MM/DD/YYYY format, WITH leading Zero's
    string getDate() const {
        string str = "";

        // Add Leading Zeros to Month & Day, if necessary
        if (m < 10) str += "0";
        str += to_string(m) + "/";

        if (d < 10) str += "0";
        str += to_string(d) + "/";

        str += to_string(y);
        return str;
    }
};


/*    Timer Struct    */
// Simple class to time how long an action takes to execute
struct Timer {

    Logger classLog;
    high_resolution_clock::time_point start, stop;
    milliseconds::rep duration = 0;

    Timer() : classLog(FUNC, 3) { start = high_resolution_clock::now(); }

    // Start & Stop the timer
    void startTimer() { start = high_resolution_clock::now(); }
    void stopTimer() {
        stop = high_resolution_clock::now();
        duration = duration_cast<milliseconds>(stop - start).count();
    }

    // Getters for the duration
    string getDurationStr() const { return to_string(duration); }
    milliseconds::rep getDuration() const { return duration; }

    // Multiply elapsed time by 'num' and return as seconds in a string
    string calculateTime(int num = 1) const {
        stringstream s;
        s << fixed << setprecision(2) << (duration * num) * 0.001;
        return s.str();
    }
};



/*    RandNo Struct    */
// Just retruns a random number between the two integers passed (lo - hi)
struct RandNo {
    // Return a Random integer between 'lo' and 'hi'
    int getRand(int lo, int hi) const {
        int num = rand() % (hi - lo + 1) + lo;
        return num;
    }
};



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
        grid(MAX_HEIGHT, vector<T>(MAX_WIDTH)) {}

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



/*    ValidateInput Struct    */
// Very rudimentary right now, but this will just validate user input.
struct ValidateInput {

    Logger classLog;
    ValidateInput() : classLog(FUNC, 3) {}

    // Wait for user to press ENTER (If 'Q' is entered, return FALSE).
    bool pressEnter(string prompt = "Press [ENTER] to continue... ") {
        cout << prompt;
        string userInput;
        getline(cin, userInput);

        if (toupper(userInput[0]) == 'Q') return false;
        return true;
    }

    // Confirm Yes-Or-No
    bool confirmYN(string prompt) {
        while (true) {
            cout << prompt;
            string userInput;
            getline(cin, userInput);

            if (userInput.empty()) {
                cout << "\nERROR: Input cannot be empty!\n";
                continue;
            }

            if (toupper(userInput[0]) == 'Y') return true;
            else if (toupper(userInput[0]) == 'N') return false;
            else cout << "\nInvalid choice entered! Must be 'Y' or 'N': ";
        }
    }

    // Validates that the integer entered is between the Lo & Hi range.
    int intRange(string prompt, int lo, int hi) {
        int userInput = 0;

        while (true) {
            try {
                cout << prompt;
                cin >> userInput;

                // Check for valid integer & within range
                if (!cin) throw std::invalid_argument("Expected an Integer!");
                if (userInput < lo || userInput > hi) {
                    throw std::invalid_argument("Invalid Number. "
                        "Must be between " + to_string(lo) + " and "
                        + to_string(hi) + ".");
                }
                else break;
            }
            catch (const invalid_argument& e) {
                cout << endl << "ERROR: " << e.what() << endl;
                cin.clear();
                cin.ignore(100, '\n');
                continue;
            }
        }
        cin.clear();
        cin.ignore(100, '\n');

        return userInput;
    }

    // Validate non-empty input & under max-char limit param (default = 30)
    string stringSize(string prompt, size_t max = 30) {
        string userInput = "";
        while (true) {
            cout << prompt;
            getline(cin, userInput);

            if (userInput.empty()) {
                cout << "\nERROR: Input cannot be empty!\n";
                continue;
            }
            if (userInput.size() >= max) {
                cout << "\nERROR: Input cannot be longer than " << max
                    << " characters!\n";
                continue;
            }
            break;
        }
        return userInput;
    }
};
/*===================<END OF UTILITIES>===================*/

/***********************************
 *    Global Simulation Symbols    *
 **********************************/
 // These are used throughout the program to represent different objects
enum SymbolKeys {
    ROBOT = 'R',

    EMPTY = '0',
    WALL = 'W',
    BATTERY = '*',
    IGNORE = 'x',

    UP = '^',
    DOWN = 'v',
    LEFT = '<',
    RIGHT = '>',
    RANDOM = '?'
};

/*==============================<STATS SECTION>==============================*/
/*    GenerationStats Struct    */
struct GenerationStats {
    double avgFitness = 0.0;
    double avgTurns = 0.0;
    double avgRounds = 0.0;
    int numBots = 0;
    int generation = 0;
    int numMutants = 0;
};


/*    RobotStats Struct    */
struct RobotStats {
    int fitness = 0;
    int turns = 0;
    int rounds = 0;
    int generation = 0;
    int mutations = 0;

    double getAvgFitness() const {
        if (rounds == 0) return 0;
        return fitness / rounds;
    }
    double getAvgTurns() const {
        if (rounds == 0) return 0;
        return turns / rounds;
    }

    RobotStats() {}
    RobotStats(const RobotStats& copy) { *this = copy; }

    RobotStats& operator=(const RobotStats& copy) {
        fitness = copy.fitness;
        turns = copy.turns;
        rounds = copy.rounds;
        generation = copy.generation;
        mutations = copy.mutations;
        return *this;
    }
};


/*    StatGraphProperties Struct    */
// This is my attempt at dynamically adjusting the graph's width and height
struct StatGraphProperties {

    // I advise against changing these values
    int MAX_GRID_HEIGHT = 20;
    int MAX_GRID_WIDTH = 25;
    int gridWidth;
    int interval = (MAX_GENERATIONS - 2) / (MAX_GRID_WIDTH - 2);

    void adjustGraph() {
        if (MAX_GENERATIONS > MAX_GRID_WIDTH) gridWidth = MAX_GRID_WIDTH;
        else gridWidth = MAX_GENERATIONS;
    }

    bool isInterval(int i) const {
        if (MAX_GENERATIONS <= MAX_GRID_WIDTH) return false;

        if (i == 0) return false;
        if (i == gridWidth - 1) return false;
        for (int j = 0; j < MAX_GENERATIONS; j += interval) {
            if (i == j) return false;
        }
        return true;
    }

    int getWidth() const { return gridWidth; }
    int getHeight() const { return MAX_GRID_HEIGHT; }

    StatGraphProperties() { adjustGraph(); }
};


/*    StatEngine Class    */
class StatEngine {

    Logger classLog;
    StatGraphProperties props;

    Grid<char> grid;
    vector<RobotStats> botStats;
    vector<GenerationStats> genStats;

public:
    StatEngine() : classLog(FUNC), grid(props.getWidth(), props.getHeight()) {}

    // Store stats to botStats vector
    void addStats(const RobotStats& newStats) { botStats.push_back(newStats); }

    GenerationStats getGenStats(size_t g) const { return genStats[g - 1]; }
    char getCell(int x, int y) const { return grid.getCell(x, y); }

    // Calculates the stats for each generation and stores them in 'genStats'
    void calculateStats() {
        Logger log(FUNC);

        sortBotStats();

        // Go through each generation and calculate respective stats
        // If MAX_GENERATIONS is set to a high number, it will skip intervals
        // to match the output-width of the graph.
        for (int g = 0; g < MAX_GENERATIONS; g++) {
            if (props.isInterval(g)) continue;

            GenerationStats currGen;
            currGen.generation = g + 1;
            double sumFitness = 0;
            double sumTurns = 0;
            int sumRounds = 0;

            // Organize each bot by generation and calculate stats
            for (size_t i = 0; i < botStats.size(); ++i) {
                RobotStats currBot = botStats[i];
                if (currBot.generation == g + 1) {
                    sumFitness += currBot.getAvgFitness();
                    sumTurns += currBot.getAvgTurns();
                    sumRounds += currBot.rounds;
                    currGen.numBots++;
                    if (currBot.mutations > 0) currGen.numMutants++;
                }
            }
            // Assign calculated stats to the current generation
            currGen.avgFitness = (sumFitness / currGen.numBots);
            currGen.avgTurns = sumTurns / currGen.numBots;
            currGen.avgRounds = sumRounds / (double)currGen.numBots;
            genStats.push_back(currGen);
        }
    }

    // Plot the calculated stats onto the graph
    void plotStats() {
        Logger log(FUNC, 4);

        calculateStats();
        grid.wipeGrid(EMPTY);
        sortGenStats();

        // Plot each generation's stats onto the graph
        for (int g = 0; g < props.getWidth(); g++) {
            GenerationStats currGen = genStats[g];
            // Adjusts the height of the bar based on the height scale
            int avg = int(round(currGen.avgFitness) / GRAPH_HEIGHT_SCALE);
            for (int i = 0; i < avg - 1; i += 1) {
                grid.setCell(g, i, '=');
            }
        }
    }

    // Sort Robot stats by Generation number
    void sortBotStats() {
        Logger log(FUNC, 4);
        std::sort(botStats.begin(), botStats.end(),
            [](const RobotStats& a, const RobotStats& b) {
                return a.generation < b.generation;
            });
    }

    // Sort Generational stats by Generation number
    void sortGenStats() {
        Logger log(FUNC, 4);
        std::sort(genStats.begin(), genStats.end(),
            [](const GenerationStats& a, const GenerationStats& b) {
                return a.generation < b.generation;
            });
    }

    StatEngine& operator=(const StatEngine& copy) {
        grid = copy.grid;
        botStats = copy.botStats;
        genStats = copy.genStats;
        return *this;
    }
};


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

    // Print a graph of the final results of the simulation
    void printResults(const StatEngine& stats) const {
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
    void printGenerationalStats(const StatEngine& stats) const {
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
    void printHeader(string s) const {
        cout << "\n" << align(COLOR(s, YELLOW, BOLD), 'C', OUTPUT_WIDTH)
            << "\n";
    }

    // Similar to printHeader, but with a border
    string titleBar(const string& title, char lChar, char rChar) const {
        string plate = string(1, lChar) + " " + title + " " + string(1, rChar);
        return "\n" + align(plate, 'C', OUTPUT_WIDTH, '=') + "\n";
    }

    string uiBar(int l, char c) const { return COLOR(string(l, c), uiColor); }
};


/*    StatController Struct    */
// Interfaces the StatViewer struct with the StatEngine class
struct StatController {

    Logger classLog;
    StatEngine track;
    StatViewer view;

    StatController() : classLog(FUNC) {}

    void printFinalGraph() {
        track.plotStats();
        view.printResults(track);
    }
    void printGenerationalStats() const { view.printGenerationalStats(track); }

    void addStats(const RobotStats& newStats) { track.addStats(newStats); }

    StatController& operator=(const StatController& copy) {
        track = copy.track;
        return *this;
    }
};


/*==============================<BOTS SECTION>===============================*/
/*    Gene Struct    */
// Houses an individual gene for a Robot (4 sensors, 1 action)
struct Gene {

    RandNo rand;

    static const int NUM_GENE_CODES = 5;
    static const int NUM_ACTIONS = 5;
    static const int NUM_SENSORS = 4;
    char action[NUM_ACTIONS] = { UP, DOWN, LEFT, RIGHT, RANDOM };
    char sensor[NUM_SENSORS] = { EMPTY, WALL, BATTERY, IGNORE };

    array<char, NUM_GENE_CODES> gene;

    // Constructors
    Gene() { generateGene(); }
    Gene(const Gene& copy) { *this = copy; }

    // Getters
    array<char, NUM_GENE_CODES> getGene() const { return gene; }
    char getGeneAt(size_t i) const { return gene[i]; }
    char getAction() const { return gene[size_t(NUM_GENE_CODES - 1)]; }

    // Return random sensor or action
    char genRandSensor() const { return sensor[rand.getRand(0, NUM_SENSORS - 1)]; }
    char genRandAction() const { return action[rand.getRand(0, NUM_ACTIONS - 1)]; }

    // Return all sensors (for map scanning)
    array<char, NUM_SENSORS> getSensors() const {
        array<char, NUM_SENSORS> sensors = { EMPTY, EMPTY, EMPTY, EMPTY };
        for (size_t i = 0; i < NUM_SENSORS; ++i) sensors[i] = gene[i];
        return sensors;
    }

    // Mutate a random node for a gene
    Gene mutateGene() const {
        // Create a copy
        Gene mutatedGene(*this);
        int pos = rand.getRand(0, NUM_GENE_CODES - 1);
        char mutatedCode = gene[pos];

        // Randomly replace one code (until it's different).
        while (mutatedCode == gene[pos]) {
            // If the mutation happens for a sensor
            if (pos < NUM_GENE_CODES - 1) mutatedCode = genRandSensor();
            // If the mutation happens for an action
            else mutatedCode = genRandAction();
        }
        mutatedGene.gene[pos] = mutatedCode;
        return mutatedGene;
    }

    // Randomly generate Gene's data
    void generateGene() {
        for (int i = 0; i < NUM_GENE_CODES - 1; ++i) gene[i] = genRandSensor();
        gene[size_t(NUM_GENE_CODES - 1)] = genRandAction();
    }

    // For Gene comparisons (Component Testing)
    bool operator==(const Gene& other) const {
        for (size_t i = 0; i < gene.size(); ++i) {
            if (gene[i] != other.gene[i]) return false;
        }
        return true;
    }

    Gene& operator=(const Gene& copy) {
        gene = copy.gene;
        return *this;
    }
};


/*    GeneManager Class    */
// Manages the set of genes for each Robot
class GeneManager {

    vector<Gene> genes;     // Stores all 16 genes

public:
    GeneManager() { generateGenes(); }
    GeneManager(const GeneManager& copy) { *this = copy; }

    // Randomly generate all genes
    void generateGenes() {
        genes.clear();
        for (int i = 0; i < GENES_PER_BOT; ++i) genes.push_back(Gene());
    }

    // Compare sensor data to stored genes and return corresponding action
    char compareGenes(array<char, 4> data) {
        for (size_t i = 0; i < genes.size(); ++i) {
            Gene curGene = genes[i];
            if (curGene.getSensors() == data) {
                char action = curGene.getAction();
                if (action != RANDOM) return curGene.getAction();
                else return curGene.genRandAction();
            }
        }
        return genes.back().getAction();    // Return last gene's action 
    }

    // Setters
    void setGeneAt(size_t i, const Gene& newGene) { genes.at(i) = newGene; }
    void setGenes(const vector<Gene>& newGenes) { genes = newGenes; }
    void addGene(const Gene& newGene) { genes.push_back(newGene); }
    void clear() { genes.clear(); }

    // Getters
    vector<Gene> getAllGenes() const { return genes; }
    Gene getGeneAt(size_t i) const { return genes[i]; }
    int getMaxGenes() const { return GENES_PER_BOT; }

    GeneManager& operator=(const GeneManager& copy) {
        genes = copy.genes;
        return *this;
    }
};


/*    Robot Class    */
class Robot {

    RandNo rand;
    GeneManager genes;
    RobotStats stats;

    int energy;
    bool alive = true;

public:
    Robot() : energy(STARTING_ENERGY) {}
    Robot(const Robot& copy) { *this = copy; }

    // Analyze map surroundings, compare with Genes, and return action.
    char processSensorData(array<char, 4> area) {
        return genes.compareGenes(area);
    }

    // Determine if the Robot is still alive
    void checkHealth() {
        if (energy > 0 || !alive) return;
        energy = 0;
        alive = false;
    }

    // Reset the Bot's base stats for a new round
    void startRound() {
        energy = STARTING_ENERGY;
        alive = true;
        stats.rounds += 1;
    }

    // Setters
    void increaseTurns() { if (alive) stats.turns++; }
    void increaseMutations() { stats.mutations++; }
    void expendEnergy(int n) { if (alive) energy -= n; }
    void setGeneration(int n) { stats.generation = n; }
    void increaseEnergy(int n) {
        if (!alive) return;
        energy += n;
        stats.fitness += FITNESS_INCREASE;
    }

    // Getters
    Gene getGeneAt(size_t i) const { return genes.getGeneAt(i); }
    GeneManager getGenes() const { return genes; }
    int getMaxGenes() const { return GENES_PER_BOT; }

    const RobotStats getStats() const { return stats; }
    double getAvgFitness() const { return stats.getAvgFitness(); }
    int getMutations() const { return stats.mutations; }
    bool isMutant() const { return (stats.mutations > 0); }
    bool isAlive() const { return alive; }

    // Overloaded '+' Operator for reproducing
    Robot operator+(const Robot& other) const {
        Logger log(FUNC, 5);
        Robot child;

        for (int i = 0; i < GENES_PER_BOT; ++i) {
            // Combine parents' genes (random parent for each gene)
            Gene childGene;
            if (rand.getRand(0, 1) == 0) childGene = genes.getGeneAt(i);
            else childGene = other.genes.getGeneAt(i);

            // Gene Mutation (5% chance by default)
            int mutationCheck = rand.getRand(1, 100);
            if (mutationCheck <= GENE_MUTATION_CHANCE) {
                childGene = childGene.mutateGene();
                child.increaseMutations();
            }
            child.genes.setGeneAt(i, childGene);
        }
        return child;
    }

    // Overloaded '=' Operator
    Robot& operator=(const Robot& old) {
        energy = old.energy;
        alive = old.alive;
        genes = old.genes;
        stats = old.stats;  // Stats are copied for the mutation count
        // No other stats are affected by this.
        return *this;
    }
};


/*==============================<MAP SECTION>================================*/
/*    MapEngine Class    */
 // Directly manages each Robot's instance of the map by using my Grid class
class MapEngine {

    Logger classLog;
    RandNo rand;
    Grid<char> map;

    // Map Properties
    static const int MAP_WIDTH = 10;
    static const int MAP_HEIGHT = 10;
    static const int NUM_CELLS = MAP_WIDTH * MAP_HEIGHT;
    int MAX_BATTERIES // Using cmath.round to reduce potential precision loss
        = static_cast<int>(round(NUM_CELLS * (STARTING_BATTERY_RATE * 0.01)));

    int numBatteries = 0;

public:
    MapEngine() : classLog(FUNC, 4), map(MAP_WIDTH, MAP_HEIGHT) { generateMap(); }

    // Randomly place Robot and then the Batteries
    void generateMap() {
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
    void emptyCell(int x, int y) { map.setCell(x, y, EMPTY); }
    void setCell(int x, int y, char v) { map.setCell(x, y, v); }
    void decreaseBatteries() { numBatteries--; }

    // Getters
    int getMapWidth() const { return MAP_WIDTH; }
    int getMapHeight() const { return MAP_HEIGHT; }
    char getCell(int x, int y) const {
        if (x < 0 || x > MAP_WIDTH - 1) return WALL;
        if (y < 0 || y > MAP_HEIGHT - 1) return WALL;
        return map.getCell(x, y);
    }

    MapEngine& operator=(const MapEngine& old) {
        numBatteries = old.numBatteries;
        map = old.map;
        return *this;
    }
};


/*    MapController Class    */
class MapController {

    MapEngine map;

    int curX, curY;     // Robot's current location
    int maxX, maxY;
    array<char, 4> area;

    // Safe Increment/Reductions to prevent out-of-bounds
    void incY() { curY = min(curY + 1, maxY); }
    void decY() { curY = max(curY - 1, 0); }
    void incX() { curX = min(curX + 1, maxX); }
    void decX() { curX = max(curX - 1, 0); }

public:
    MapController() {
        maxX = map.getMapWidth() - 1;
        maxY = map.getMapHeight() - 1;
        updateRobotLocation();
    }

    // Move Robot from its current position, depending on its decision
    bool moveRobot(char d) {
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
    array<char, 4> scan() {
        area[0] = map.getCell(curX, curY + 1);  // Up
        area[1] = map.getCell(curX, curY - 1);  // Down
        area[2] = map.getCell(curX - 1, curY);  // Left
        area[3] = map.getCell(curX + 1, curY);  // Right
        return area;
    }

    void consumeBatteryAt(int x, int y) {
        map.emptyCell(x, y);
        map.decreaseBatteries();
    }

    void updateRobotLocation() {
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

    void resetMap() {
        map.generateMap();
        updateRobotLocation();
    }

    MapController& operator=(const MapController& old) {
        map = old.map;
        curX = old.curX;
        curY = old.curY;
        area = old.area;

        return *this;
    }
};


/*===========================<BOT/MAP INTERFACE>=============================*/
/*    RobotController Class    */
// Interfaces Robots with Maps
struct RobotController {

    Logger classLog;
    MapController map;
    Robot bot;

    static const int BATTERY_BOOST = 5;    // Energy gained from batteries
    static const int ENERGY_COST = 1;      // Cost of moving

    // Functions
    RobotController() : classLog(FUNC, 4) {}
    RobotController(const RobotController& copy) { *this = copy; }

    // Main function that cycles through a bot's turn
    void botCycle() {
        Logger log(FUNC, 6);

        // Reset the Map & the Bot's stats to test it again
        bot.startRound();
        map.resetMap();

        // While the Bot is alive it will search for batteries until it dies.
        while (bot.isAlive()) {
            bot.expendEnergy(ENERGY_COST);

            // Map scans area, Bot processes sensor data, Map moves bot.
            char move = bot.processSensorData(map.scan());
            if (map.moveRobot(move)) {
                bot.increaseEnergy(BATTERY_BOOST);
            }
            // Only increase turns if the bot is still alive
            bot.checkHealth();
            if (bot.isAlive()) bot.increaseTurns();
        }
    }

    RobotController operator+(const RobotController& other) const {
        RobotController child;
        child.bot = bot + other.bot;
        return child;
    }

    RobotController& operator=(const RobotController& old) {
        map = old.map;
        bot = old.bot;
        return *this;
    }
};


/*==============================<SIMULATION SECTION>=========================*/
/**************************
 *    Simulation Class    *
 *************************/
 // Houses the core simulation loop
class Simulation {

    Logger classLog;
    Timer timer;
    vector<shared_ptr<RobotController>> bots;

    int currGen = 0;
    const double REPORT_INTERVAL = MAX_GENERATIONS
        * (PROGRESS_UPDATE_RATE * 0.01);

public:
    StatController stats; // Public for ease-of-access by SimMenu

    // Constructors/Destructors
    Simulation() : classLog(FUNC) {}
    Simulation(const Simulation& copy) { *this = copy; }
    ~Simulation() { bots.clear(); }

    void beginSim() {
        Logger log(FUNC);

        Timer simTimer; // Time the simulation runtime

        // Cycle through each generation
        simCycle();

        // Delete remaining bots & add their stats
        purgeRemainingBots();

        // Print Stats
        stats.printFinalGraph();

        // Print total simulation time
        simTimer.stopTimer();
        cout << COLOR("Total Time: ", CYAN)
            << COLOR(simTimer.calculateTime(), YELLOW)
            << COLOR(" seconds\n\n", BLACK, BRIGHT);
    }

    // Cycles through each generation, performing the core events
    void simCycle() {
        Logger log(FUNC);

        while (currGen < MAX_GENERATIONS) {
            if (REPORT_INTERVAL > 0) timer.startTimer();

            currGen++;
            newGeneration();
            generationCycle();

            // Progress report
            if (REPORT_INTERVAL < 1) continue;
            if (currGen % int(REPORT_INTERVAL) == 0) {
                timer.stopTimer();
                progressReport();
            }
        }
    }

    // Cycles through each bot in the current generation
    void generationCycle() {
        Logger log(FUNC);
        log.logTime("Generation " + to_string(currGen) + " Started");

        for (size_t i = 0; i < bots.size(); ++i)
            bots[i]->botCycle();
    }

    // Creates a new generation of bots
    void newGeneration() {

        // Initialize the first generation
        if (currGen == 1) {
            initializePopulation();
            return;
        }

        // Prevent the final generation from breeding
        if (currGen > MAX_GENERATIONS) return;

        // Sort, then purge + breed based on performance
        sortBots();
        purgePopulation();
        breedPopulation();
    }

    // Thanos-Snap the bottom 50% of performers
    void purgePopulation() {
        Logger log(FUNC);
        size_t midway = MAX_POPULATION / 2;

        while (bots.size() > midway && !bots.empty()) {
            addBotStats(bots.back());   // Log bot's stats before deletion
            bots.pop_back();
        }
    }

    // Breed the top 50% of performers
    void breedPopulation() {
        Logger log(FUNC);

        size_t midway = MAX_POPULATION / 2;
        size_t i = 0;
        while (i < midway && bots.size() < size_t(MAX_POPULATION - 1)) {

            shared_ptr<RobotController> child1 = make_shared
                <RobotController>(*(bots.at(i)) + *(bots.at(i + 1)));
            child1->bot.setGeneration(currGen);
            bots.push_back(child1);

            shared_ptr<RobotController> child2 = make_shared
                <RobotController>(*(bots.at(i)) + *(bots.at(i + 1)));
            child2->bot.setGeneration(currGen);
            bots.push_back(child2);

            i += 2;
        }
    }

    // Log a Robot's stats to the Stat-Tracker
    void addBotStats(const shared_ptr<RobotController>& bot) {
        Logger log(FUNC, 5);
        RobotStats curStats = bot->bot.getStats();
        stats.addStats(curStats);
    }

    // Logs all the remaining Bots to the Stat-Tracker
    void purgeRemainingBots() {
        Logger log(FUNC);
        while (!bots.empty()) {
            addBotStats(bots.back());
            bots.pop_back();
        }
    }

    // Sort Bots by average fitness
    void sortBots() {
        Logger log(FUNC, 4);
        std::sort(bots.begin(), bots.end(),
            [](const shared_ptr<RobotController>& a,
                const shared_ptr<RobotController>& b) {
                    return a->bot.getAvgFitness() > b->bot.getAvgFitness();
            });
    }

    void clearBots() { bots.clear(); }

    // Initialize the first generation of bots with random genes
    void initializePopulation() {
        Logger log(FUNC);

        for (size_t i = 0; i < MAX_POPULATION; ++i) {
            shared_ptr<RobotController> newBot = make_shared<RobotController>();
            newBot->bot.setGeneration(currGen);
            bots.push_back(newBot);
        }
    }

    // Updates the user on the simulation's progress
    void progressReport() const {
        FormatText ft;

        // Clear previous progress report
        if (currGen > REPORT_INTERVAL) ft.clearLines(3);
        if (currGen > REPORT_INTERVAL * 3) ft.clearLines(1);

        const int WIDTH = 40;
        cout << "\n" << align("Simulation Progress:", 'C', WIDTH) << endl;
        cout << ft.statusBar(currGen, MAX_GENERATIONS, WIDTH) << endl;

        if (currGen > REPORT_INTERVAL * 2) {
            int calcFormula = int(round((MAX_GENERATIONS - currGen)));
            string calcTime = timer.calculateTime(calcFormula);
            cout << align(COLOR("Estimated Time: ", CYAN), 'R', WIDTH / 2 + 5)
                << COLOR(calcTime, YELLOW) << " s\n";
        }
    }

    int getCurrGen() const { return currGen; }

    Simulation& operator=(const Simulation& copy) {
        if (this != &copy) {
            currGen = copy.currGen;
            bots = copy.bots;
            stats = copy.stats;
        }
        return *this;
    }
};


/*    Simulation Menu Class    */
class SimulationMenu {

    Logger classLog;
    Simulation sim;
    ValidateInput getInput;

    bool quitFlag = false;

public:
    SimulationMenu() : classLog(FUNC) {}
    ~SimulationMenu() {}

    // Extremely rudimentary Main menu for the Simulation
    void mainMenu() {
        Logger log(FUNC);

        printIntro();

        // Loop main menu until the user quits
        while (!quitFlag) {

            string userInput = "";
            cout << COLOR("\nChoose an option below:\n", CYAN);
            printMainMenu();

            cout << "Enter Option here: ";
            getline(cin, userInput);

            switch (toupper(userInput[0])) {
            case 'A':   // Run Simulation
                runSim();
                if (!getInput.pressEnter()) return;
                break;
            case 'B':   // View Stats
                if (sim.getCurrGen() == 0) {
                    cout << "\nYou must run a Simulation first!\n";
                    continue;
                }
                viewStats();
                if (!getInput.pressEnter()) return;
                break;
            case 'Q':   // Quit
                quitFlag = true;
                break;
            default:
                cout << "\nInvalid Option!\n";
                continue;
            }
        }
    }

    // Completely re-instantiates the Simulation Object
    void resetSim() {
        Logger log(FUNC);
        sim = Simulation();
    }

    // Run a new Simulation. If the Simulation has already been run, reset it.
    void runSim() {
        Logger log(FUNC);

        if (sim.getCurrGen() > 0) {
            cout << "Resetting Simulation...\n";
            resetSim();
        }
        cout << "\nRunning Simulation (" + to_string(MAX_GENERATIONS)
            + " Generations) ...\n";
        sim.beginSim();
    }

    // View more detailed Generational stats from latest Simulation run
    void viewStats() const {
        Logger log(FUNC);

        if (sim.getCurrGen() == 0) {
            cout << "\nYou must run a Simulation first!\n";
            return;
        }
        sim.stats.printGenerationalStats();
    }

    void printMainMenu() const {
        cout << "[" << COLOR('A', GREEN, BRIGHT) << "] "
            << "Run a New Simulation" << endl;
        cout << "[" << COLOR('B', GREEN, BRIGHT) << "] "
            << "View Each Generation's Stats" << endl;
        cout << "[" << COLOR('Q', RED, BRIGHT) << "] "
            << "Quit" << endl;
    }

    // Finally revisited my WrapText() function to make it more versatile. 
    // It's not thoroughly tested, but it seems to work well here.
    void printIntro() const {
        FormatText introFt(2);
        const int WIDTH = 70;

        string str = COLOR("A Simulation of Evolution ", GREEN, BRIGHT) +
            "\nThis program will use a population of Robots to simulate "
            "the effects of evolution over time.\n\n";
        str += COLOR("The Robots: ", BLUE, BRIGHT) +
            "Each Robot will be given a set of Genes that "
            "will dictate how they interact with their environment. "
            "The Robots will then compete by navigating a random map to "
            "see who can collect the most batteries.\n\n";
        str += COLOR("The Simulation: ", BLUE, BRIGHT) +
            "At the end of each round, the Bots will be sorted by how "
            "successful they were at harvesting energy that round (Fitness). "
            "The bottom 50% will be eliminated from the simulation and "
            "the top 50% will be allowed to breed, combining their superior "
            "genes. This process will repeat until the maximum number of "
            "generations has been reached.\n\n";
        str += COLOR("The Statistics: ", BLUE, BRIGHT) +
            "At the end of the simulation, you will be able to view the "
            "trends for each Generation's average fitness scores.";

        cout << "\n" << introFt.wrapText(str, 'L', WIDTH) << "\n\n";
    }
};

/*================================<PROTOTYPES>===============================*/

void SimulationStarter();
void ProgramGreeting();

/*==================================<MAIN>===================================*/
int main() {
    Logger log(FUNC);
    srand((unsigned)time(NULL));

    ProgramGreeting();

    SimulationStarter();

    cout << COLOR("\n\nProgram Ended\n", RED, BRIGHT);
    return 0;
}

// Begins the Simulation. Separated into a standalone function so the Logger
// displays accurate an sequence of events.
void SimulationStarter() {
    Logger log(FUNC);

    SimulationMenu sim;
    sim.mainMenu();
}

void ProgramGreeting() {
    Logger log(FUNC);

    const string projName = COLOR("FinalProj.cpp", YELLOW);
    Date date;
    cout << "+-----------------------------------+\n"
        << "| " << projName << ", by Ian Featherston |\n"
        << "| CISP 400, " << date.getDate() << "              |\n"
        << "+-----------------------------------+\n";
}