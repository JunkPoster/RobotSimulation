/*
Logger v04.29
- MACROS REMOVES for stability and portability
- Detects beginning of program to start new Logfile
- Now allows specifying priority, to determine inclusion.
- End-Logging is not necessary, now automatically done in ~Logger()

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
    
    
WARNING: Doing 'Logger(FUNC);' will NOT work properly.

*/

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>
using namespace std; 
using namespace chrono;

/***********************
*    Logger Classes    *
***********************/
// Which priority lvl to include up-to (Higher = more logging)
const int LOGGING_LEVEL = 2;

// This fix is for VisualStudio, as it doesn't use __PRETTY_FUNCTION__
#ifdef _MSC_VER 
#define __PRETTY_FUNCTION__ __FUNCSIG__ 
#endif 
// Allows the passing of a Function's name as 'FUNC'
#define FUNC __PRETTY_FUNCTION__

// Static instance to reduce file IO activity
class LoggerFileHandler {

    const string FILENAME = "log.txt";
    ofstream file;
    
public:
    LoggerFileHandler() { file.open(FILENAME, ios_base::out); }
    ~LoggerFileHandler() { if (file.is_open()) file.close(); }
    
    void openFile() { if (!file.is_open()) file.open(FILENAME, ios_base::out); }
    void closeFile() { if (file.is_open()) file.close(); }
    void writeToLogFile(const string& msg) { 
        
        /* TODO (#1#): REMOVE OPEN/CLOSE BEFORE SUBMIT */
        
        file << msg;
        closeFile();
        file.open(FILENAME, ios_base::app);
    }
    
    LoggerFileHandler(const LoggerFileHandler&) = delete;
    LoggerFileHandler& operator=(const LoggerFileHandler&) = delete;
};

// Global File-Handler Instance
LoggerFileHandler logfileHandler;

class Logger {
    
    // Define amount of logging activity. Each number includes those before it.
    const int INCLUSION_LVL = LOGGING_LEVEL;
    
    int eventPriority = 0;  // Sets Priority level of current event
    string eventType;       // Standalone, Class, Member, etc.
    string eventAction;     // Specifies event actions (Called, Ended, etc.)
    
    string funcName;        // Function name automatically passed using Macros
    char stamp[25];         // Contains timestamps
    
    time_t curTime;
    struct tm localTime;
    high_resolution_clock::time_point start, stop;
    
    static bool startOfProgram;     // Performs Start-Of-File formatting
    static bool loggingEnabled;     // Can be used to toggle Logging
    static string baseFunction;     // Starter Function's name to end logging
    
    // Start a new file with unique Header
    void startNewFile() {
        time(&curTime);
        localtime_s(&localTime, &curTime);

        // Format file header
        strftime(stamp, sizeof(stamp), "%A, %m/%d/%Y", &localTime);
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
        } else {
            prefix = str.substr(0, pos);
            suffix = str.substr(pos + 2);
            if (prefix == suffix) {
                funcName = prefix;
                eventType = "CLASS";
            } else {
                funcName = "  " + str;          // Apply minor spacing
                eventType = "MEMBER";
            }
        }
        
        // If a Template is detected, alter behavior
        str = rawFuncName; // reset to original string
        pos = str.find("<");
        if (pos != string::npos) {
            size_t pos2 = str.find(")") + 1;
            str = str.replace(pos + 1, pos2 - pos, "");
            pos2 = str.find("=");
            str = str.replace(pos + 2, pos2 - pos, "");
            funcName = str + ">";
            eventPriority = 0;
        }
    }
    
    // Convert and return exact timestamp as string
    string timeStamp() {
        auto now = system_clock::now();
        auto now_2 = system_clock::to_time_t(now);
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        
        time(&curTime);
        localtime_s(&localTime, &curTime);
        strftime(stamp, sizeof(stamp), "[%H:%M:%S.", &localTime);
        snprintf(stamp + strlen(stamp), sizeof(stamp) - strlen(stamp), 
                                "%03d] ", static_cast<int>(ms.count()));
        return stamp;
    }
    
public:
    Logger() {
        time(&curTime);
        localtime_s(&localTime, &curTime);
        strftime(stamp, sizeof(stamp), "[%H:%M:%S", &localTime);
    }
    // An integer can be passed to manually set the priority
    Logger(string funcName, int prio = 0, string msg = "") 
        : eventPriority(prio) { startLog(funcName, msg); }
        
    ~Logger() { if (funcName != "") endLog(); }
    
    void writeToFile(const string& logMsg) const {
        if (INCLUSION_LVL < eventPriority) return;
        logfileHandler.writeToLogFile(logMsg);
    }
    
    // Logs the Start of a function
    void startLog(const string& name, const string& msg = "") {
        start = high_resolution_clock::now();
        
        // Format the current function's name, type, & action
        formatFuncName(name);
        formatFuncAction();
        formatEventPriority();
        
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

/******************************************************************************/