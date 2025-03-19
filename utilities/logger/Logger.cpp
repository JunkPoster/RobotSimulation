/*============================================================================*
File: Logger.cpp
Desc: Implementation of the Logger class. This class is responsible for logging
        the start and end of functions, as well as any custom messages.
*============================================================================*/

#include "Logger.h"

// Initialize static variables
bool Logger::loggingEnabled = true;
bool Logger::startOfProgram = true;
string Logger::baseFunction = "";


// ============================ LOGGER CLASS =============================== //
Logger::Logger() : curTime(time(0)) {
    timeStamp();
}

// An integer can be passed to manually set the priority
Logger::Logger(string func, int prio, string msg) : eventPriority(prio) {
    startLog(func, msg);
}

Logger::~Logger() {
    if (funcName != "") endLog();
}


// Start a new file with unique Header
void Logger::startNewFile() {

    // Format file header
    localtime_s(&local, &curTime);
    strftime(stamp, sizeof(stamp), "%A, %m/%d/%Y", &local);
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
void Logger::endFile() {
    string endMsg = "\n" + string(35, '=') + "\n"
        + timeStamp() + "PROGRAM ENDED ";
    writeToFile(endMsg);
}

// Assigns the Action based on type (Called, Constructed, Returned, etc.)
void Logger::formatFuncAction(const string& str) {
    // Adjust funcAction if endLog() called, then return
    if (str == "!end") {
        if (eventType == "CLASS") eventAction = "Destroyed";
        else eventAction = "Returned";
        return;
    }

    eventAction = "Called";      // Default action
    if (eventType == "CLASS") eventAction = "Constructed";
}

void Logger::formatEventPriority() {
    if (eventPriority != 0) return;
    if (eventType == "STANDALONE") eventPriority = 0;
    if (eventType == "CLASS") eventPriority = 1;
    if (eventType == "MEMBER") eventPriority = 2;
}

// Strip return type and parenthesis from __PRETTY_FUNCTION__
void Logger::formatFuncName(const string& rawFuncName) {

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
string Logger::timeStamp() {
    auto now = system_clock::now();
    time_t now_2 = system_clock::to_time_t(now);
    tm tmLocal = {};
    localtime_s(&tmLocal, &now_2);

    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    strftime(stamp, sizeof(stamp), "[%H:%M:%S.", &tmLocal);
    snprintf(stamp + strlen(stamp), sizeof(stamp) - strlen(stamp),
            "%03d] ", static_cast<int>(ms.count()));

    return stamp;
}

void Logger::writeToFile(const string& logMsg) const {
    if (INCLUSION_LVL < eventPriority) return;
    LoggerFileHandler::instance().writeToLogFile(logMsg);
}

// Logs the Start of a function
void Logger::startLog(const string& name, const string& msg) {

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
void Logger::endLog(const string& msg) {
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
void Logger::logTime(const string& msg) { logTime(0, msg); }
void Logger::logTime(int prio, const string& msg) {
    eventPriority = prio;
    writeToFile(timeStamp() + "[] | " + string(prio, ' ') + msg + "\n");
}

// Logs PASS/FAIL status for a ComponentTest().
void Logger::logTestStatus(const string& testName, bool testPassed) const {
    string logMsg = string(testPassed ? "[PASSED] " : "[FAILED] ")
        + testName + " Testing Finished.\n\n";
    writeToFile(logMsg);
}

// Logs specific location of Component failures & error message
void Logger::logErrorMsg(const string& testName, const string& errorMsg) {
    string logMsg = timeStamp() + "!!!| Failure Happened in "
        + testName + "\n" + string(18, ' ') + "| ";
    if (errorMsg != "") logMsg += "-- " + errorMsg + "\n";
    else logMsg += "-- No Error Message Provided.\n";

    writeToFile(logMsg);
}

void Logger::setLoggingStatus(bool flag) { loggingEnabled = flag; }