/*============================================================================*
File: ValidateInput.h
Desc: Contains a struct that will validate user input.
NOTE: It's extremely rudimentary right now, but it works fine for this.
*============================================================================*/

#pragma once

#include <iostream>
#include <string>

using namespace std;

/*    ValidateInput Struct    */
// Very rudimentary right now, but this will just validate user input.
struct ValidateInput {

    ValidateInput() {}

    // Wait for user to press ENTER (If 'Q' is entered, return FALSE).
    bool pressEnter(string prompt = "Press [ENTER] to continue... ") {
        cout << prompt;
        string userInput;
        getline(cin, userInput);

        if (toupper(userInput[0]) == 'Q') return false;
        return true;
    }

    // Confirm Yes-Or-No
    bool confirmYN(string prompt) {
        while (true) {
            cout << prompt;
            string userInput;
            getline(cin, userInput);

            if (userInput.empty()) {
                cout << "\nERROR: Input cannot be empty!\n";
                continue;
            }

            if (toupper(userInput[0]) == 'Y') return true;
            else if (toupper(userInput[0]) == 'N') return false;
            else cout << "\nInvalid choice entered! Must be 'Y' or 'N': ";
        }
    }

    // Validates that the integer entered is between the Lo & Hi range.
    int intRange(string prompt, int lo, int hi) {
        int userInput = 0;

        while (true) {
            try {
                cout << prompt;
                cin >> userInput;

                // Check for valid integer & within range
                if (!cin) throw std::invalid_argument("Expected an Integer!");
                if (userInput < lo || userInput > hi) {
                    throw std::invalid_argument("Invalid Number. "
                        "Must be between " + to_string(lo) + " and "
                        + to_string(hi) + ".");
                }
                else break;
            }
            catch (const invalid_argument& e) {
                cout << endl << "ERROR: " << e.what() << endl;
                cin.clear();
                cin.ignore(100, '\n');
                continue;
            }
        }
        cin.clear();
        cin.ignore(100, '\n');

        return userInput;
    }

    // Validate non-empty input & under max-char limit param (default = 30)
    string stringSize(string prompt, size_t max = 30) {
        string userInput = "";
        while (true) {
            cout << prompt;
            getline(cin, userInput);

            if (userInput.empty()) {
                cout << "\nERROR: Input cannot be empty!\n";
                continue;
            }
            if (userInput.size() >= max) {
                cout << "\nERROR: Input cannot be longer than " << max
                    << " characters!\n";
                continue;
            }
            break;
        }
        return userInput;
    }
};
