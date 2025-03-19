/*============================================================================*
File: Logger.h
Desc: Header file for the Logger class. This class is responsible for logging
    the start and end of functions, as well as any custom messages.

NOTES:
    Inclusion Levels:
        0 - Standalone Functions
        1 - Class Cons/Destructors
        2 - Member Functions
        n - Any number can be logged as 'STARTLOG(n)' or 'LOG(n)'
    
    Implementation: {} = optional
        Function Log:
            'Logger log(FUNC, {prio})'
                or  'Logger log(__PRETTY_FUNCTION__, {prio})'
        Class Log:
            // Declare Logger classLog;
            'ClassConstructor() : classLog(FUNC) {}'
        Misc:
            Msg: 'log.writeToFile("...");'
            Time: 'log.logTime({prio}, msg);'
            Error: 'log.logErrorMsg("testName", "errorMsg");'
            Test: 'log.logTestStatus("testName", bool);'
        For Non-Initialized Misc:
            Logger().methodName("args");

*============================================================================*/

#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

#include "LoggerFileHandler.h"

using namespace std; 
using namespace chrono;


// Specify which priority lvl to include up-to: Higher = more logging, -1 = OFF
// You can read more about each Logging level in the Logger class.
/* NOTE: Logging was kept fairly low-profile for this program, and it will
    STILL produce tens-of-thousands of lines in the logfile.
    !! Lower Logging levels will have better performance. */
const int LOGGING_LEVEL = 2;

// This fix is for VisualStudio, as it doesn't use __PRETTY_FUNCTION__
#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__ 
#endif
// Allows the passing of a Function's name as 'FUNC'
#define FUNC __PRETTY_FUNCTION__


/***********************
*     Logger Class     *
***********************/
// ============================== LOGGER CLASS ==============================//
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
    tm local = {};
    high_resolution_clock::time_point start, stop;

    static bool startOfProgram;     // Performs Start-Of-File formatting
    static bool loggingEnabled;     // Can be used to toggle Logging
    static string baseFunction;     // Starter Function's name for EOF logging

    void startNewFile();
    void endFile();
    void formatFuncAction(const string& str = "");
    void formatEventPriority();
    void formatFuncName(const string& rawFuncName);
    string timeStamp();

public:
    // Constructors
    Logger();
    Logger(string func, int prio = 0, string msg = "");
    ~Logger();

    void writeToFile(const string& logMsg) const;
    void startLog(const string& name, const string& msg = "");
    void endLog(const string& msg = "");
    void logTime(const string& msg);
    void logTime(int prio, const string& msg);
    void logTestStatus(const string& testName, bool testPassed) const;
    void logErrorMsg(const string& testName, const string& errorMsg = "");
    void setLoggingStatus(bool flag);
};

/******************************************************************************/