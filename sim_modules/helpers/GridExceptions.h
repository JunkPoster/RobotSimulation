/*============================================================================*
File: GridExceptions.h
Desc: This file contains custom exceptions for the Grid class.
NOTE: I created this exception because I was having trouble with the Grid class
      throwing exceptions when trying to access an invalid index.
*============================================================================*/

#pragma once

#include <iostream>
#include <exception>
#include <string>

/*===========================<CUSTOM EXCEPTIONS>=============================*/
// This exception is thrown when trying to access an invalid index in Grid().
struct grid_out_of_bounds : public std::exception {

    grid_out_of_bounds(const string& arg) { 
        errorMsg(arg); 
    }

    void errorMsg(const string& arg) {
        cout << "\n[GRID] ERROR: Invalid Index Requested! (" + arg + ")\n";
    }
};