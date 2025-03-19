/*============================================================================*
File: DateClass.h
Desc: Contains the Date class, which stores the system time into strings.
*============================================================================*/

#pragma once

#include <iostream>
#include <ctime>
#include <string>

using namespace std;

/*******************
*    Date Class    *
*******************/
/*    Date Class    */
// Stores the System time into strings. Returns date in MM/DD/YYYY format
class Date {

    time_t now;
    tm local;
    int m, d, y;

public:
    Date();
    ~Date();

    void setDateVariables();
    string getDate() const;
};