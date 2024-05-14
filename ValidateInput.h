#include <iostream>
#include <string>
using namespace std;

/*****************************
*    ValidateInput Struct    *
*****************************/
// Very rudimentary right now, but this will just validate user input.
struct ValidateInput {
    
    // Wait for user to press ENTER to continue... (If 'Q' is entered, RETURN).
    bool pressEnter(string prompt = "Press [ENTER] to continue... ") {
        string userInput;
        cout << prompt;
        getline(cin, userInput);
        char charInput = toupper(userInput[0]);
        
        if (charInput == 'Q') return false;
        return true;
    }
    
    // Confirm Yes-Or-No
    bool confirmYN(string prompt) {
        while (true) {
            string userInput;
            cout << prompt;
            getline(cin, userInput);
            char charInput = toupper(userInput[0]);
            
            if (charInput == 'Y') return true;
            else if (charInput == 'N') return false;
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
                
                if (!cin) 
                    throw std::invalid_argument("ERROR: Expected an Integer!");
                
                if (userInput < lo || userInput > hi) {
                    cout << "\nERROR: Invalid Entry.\n";
                    cout << "Must be a number between " 
                            << lo << " and " << hi << ".\n\n";
                    cin.clear();
                    cin.ignore(100, '\n');
                    continue;
                } else break;
            // Throw exception when non-number detected
            } catch (const invalid_argument& e) {
                cout << endl << e.what() << endl;
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

ValidateInput getInput;