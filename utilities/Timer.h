/*============================================================================*
File: Timer.h
Desc: Contains a simple struct to time how long an action takes to execute.
*============================================================================*/

#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>

using namespace std;
using namespace chrono;

/*    Timer Struct    */
// Simple class to time how long an action takes to execute
struct Timer {

    high_resolution_clock::time_point start, stop;
    milliseconds::rep duration = 0;

    Timer() { 
        start = high_resolution_clock::now(); 
    }

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