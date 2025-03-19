/*============================================================================*
File: DateClass.cpp
Desc: This file contains the implementation of the Date class. It's a simple
        class that stores the current system time into strings. It's used to
        display the current date in MM/DD/YYYY format.
*============================================================================*/

#include "DateClass.h"

Date::Date() { setDateVariables(); }
Date::~Date() {}

// Convert current system time to usable data (strings)
void Date::setDateVariables() {
    now = time(0);
    localtime_s(&local, &now);
    char mon[6], day[6], year[10];
    strftime(mon, 6, "%m", &local);
    strftime(day, 6, "%d", &local);
    strftime(year, 10, "%Y", &local);

    // Assign to mon/day/year variables
    m = atoi(mon), d = atoi(day), y = atoi(year);
}

// Return current date in MM/DD/YYYY format, WITH leading Zero's
string Date::getDate() const {
    string str = "";

    // Add Leading Zeros to Month & Day, if necessary
    if (m < 10) str += "0";
    str += to_string(m) + "/";

    if (d < 10) str += "0";
    str += to_string(d) + "/";

    str += to_string(y);
    return str;
}