/*============================================================================* 
File: RandNo.h
Desc: A simple struct that just returns a random number within a given range.
*============================================================================*/

#pragma once

#include <iostream>
#include <ctime>

using namespace std;

/*    RandNo Struct    */
// Just retruns a random number between the two integers passed (lo - hi)
struct RandNo {
    // Return a Random integer between 'lo' and 'hi'
    int getRand(int lo, int hi) const {
        int num = rand() % (hi - lo + 1) + lo;
        return num;
    }
  /*
    bool ComponentTest() const {
        
        // Test Range
        for (int i = 0; i < 50; ++i) {
            int num = getRand(5, 10);
            if (num >= 5 && num <= 10) continue;
            
            //LOGERROR("getRand()", "Range Condition Incorrectly Set!");
            return false;
        }
        
        // Test Randomness (Odds of false-negative is 1:2.8 billion
        for (int i = 0; i < 2; i++) {
            int num1 = getRand(-1410065407, 1410065407);
            int num2 = getRand(-1410065407, 1410065407);
            if (num1 != num2) continue;
            
            //LOGERROR("getRand()", "Generated Numbers are NOT Random!");
            return false;
        }
        
        return true;
    }*/
};