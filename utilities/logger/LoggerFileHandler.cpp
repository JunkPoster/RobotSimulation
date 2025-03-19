/*============================================================================*
File: LoggerFileHandler.cpp
Desc: Implementation file for the LoggerFileHandler class.
*============================================================================*/

#include "LoggerFileHandler.h"

// ==================== LOGGER FILE-HANDLER CLASS ========================== //
LoggerFileHandler::LoggerFileHandler() {
    file.open(FILENAME, ios_base::out);
}

LoggerFileHandler::~LoggerFileHandler() {
    if (file.is_open()) file.close();
}

LoggerFileHandler& LoggerFileHandler::instance() {
    static LoggerFileHandler instance;
    return instance;
}

void LoggerFileHandler::startFile() {
    if (!file.is_open()) file.open(FILENAME, ios_base::out);
}

void LoggerFileHandler::appendFile() {
    if (!file.is_open()) file.open(FILENAME, ios_base::app);
}

void LoggerFileHandler::closeFile() {
    if (file.is_open()) file.close();
}

void LoggerFileHandler::writeToLogFile(const string& msg) {
    if (!file.is_open()) startFile();
    file << msg;
}