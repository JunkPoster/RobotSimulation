/*============================================================================*
File: LoggerFileHandler.h
Desc: Header file for the LoggerFileHandler class. This class is responsible for
        handling the file I/O for the Logger class. It uses the Singleton
        design pattern to ensure only one instance is created.
*============================================================================*/

#pragma once

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Static instance to reduce file IO activity
class LoggerFileHandler {

    const string FILENAME = "log.txt";
    ofstream file;

    LoggerFileHandler();
    ~LoggerFileHandler();

public:
    // Singleton Instance
    static LoggerFileHandler& instance();

    void startFile();
    void appendFile();
    void closeFile();
    void writeToLogFile(const string& msg);

    LoggerFileHandler(const LoggerFileHandler&) = delete;
    LoggerFileHandler& operator=(const LoggerFileHandler&) = delete;
};