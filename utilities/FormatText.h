/*============================================================================*
File: FormatText.h
Desc: This file contains a collection of classes and functions that allow 
    for the formatting of text in a console application. It includes 
    the following classes:
    - AlignText: Aligns text to the left/right/center of a specified width.
    - WrapText: Wraps text at a specified width, with optional indentation.
    - StatusBar: Prints a colorized status/progress bar based on the params.
    - FormatText: Allows control of all Text-Altering classes in one object.
    The file also contains a global function 'align()' that allows for the
    alignment of text without creating an object of the FormatText class.

NOTE: This file has seen a lot of iterations over multiple of my projects, but 
I'm extremely happy with how it's turned out. 
*============================================================================*/

#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "ColorText.h"

using namespace std;


/*    AlignText Class    */
struct AlignText {

    // Function to align text Left, Right, or Center
    // W = max-width that the string can be, L = length of the string
    string alignText(string str, char align, int W, char padChar = ' ') const {

        // Calculate padding based on alignment
        // C/R/L = Center/Right/Left-Aligned, respectively
        int L = static_cast<int>(visibleLength(str));
        int padLeft = 0, padRight = 0;

        switch (align) {
        case 'C':
            // Formula to center text by padding both sides.
            padLeft = padRight = (W - L) / 2 + ((W - L) % 2 != 0);

            // Adjust padding when mixing Odds/Evens.
            if (L % 2 == 0 && W % 2 != 0) padLeft -= 1;
            if (L % 2 != 0 && W % 2 == 0) padLeft -= 1;
            break;
        case 'R':
            padLeft = W - L;
            break;
        case 'L':
            padRight = W - L;
            break;
        }
        // Return string with appropriate padding
        return string(padLeft, padChar) + str + string(padRight, padChar);
    }

    // Overloaded to cast 'width' to int for alignText()
    string alignText(string str, char align, size_t W, char padChar = ' ')
        const {
        int intWidth = static_cast<int>(W);
        return AlignText().alignText(str, align, intWidth, padChar);
    }

    // Helper Function to adjust padding for colored text
    size_t visibleLength(const string& str) const {
        size_t length = 0;
        size_t i = 0;

        // Cycle through str, invisible vs. visible chars
        while (i < str.length()) {
            if (str[i] == '\033') {     // ANSI Code starts.
                while (i < str.length() && str[i] != 'm') ++i;
                ++i;
            }
            else {    // Normal characters increase length
                ++length;
                ++i;
            }
        }
        return length;
    }
};


/*    WrapText Class    */
// Child class to AlignText, which wraps text at a specified width.
struct WrapText : public AlignText {

    size_t INDENT_AMT;    // Amount of Indentation
    size_t TAB_SIZE = 4;  // Size of a Tab
    bool INDENT;          // Adjusts for Indentation behavior

    // Constructor to initialize indent behavior.
    WrapText(size_t indentAmt = 0) : INDENT_AMT(indentAmt) {
        if (INDENT_AMT > 0) {
            INDENT = true;
            INDENT_AMT = indentAmt - 1;
        }
        else {
            INDENT = false;
            INDENT_AMT = 0;
        }
    }

    // Overloaded to cast 'width' to size_t for wrapText()
    string wrapText(string str, char align, int tgtWidth, string fix = "\n")
        const {
        size_t width = static_cast<size_t>(tgtWidth);
        string newStr = wrapText(str, align, width, fix);
        return newStr;
    }

    // This word-wraps text at a specified width. Uses align().
    string wrapText(string str, char align, size_t width, string fix = "\n")
        const {

        // Adjust for '\n' and '\t' characters
        size_t newlines = countNewLineChars(str);
        replaceTabChars(str);

        // Variables for Word-Wrapping functionality
        size_t wraps;                       // Number of lines needed
        size_t length = visibleLength(str); // Counts visible characters
        string tempStr = str;               // Substringing of 'str'
        vector<string> cutStr;              // Stores substrings of 'tempStr'

        // Calculate 'wraps', which is the number of lines needed.
        if (newlines > 0) wraps = length / (width - 2) + newlines + 5;
        else wraps = length / (width - 2) + 5;

        // If the string is shorter than the width, return it as-is.
        if (length < width && newlines == 0) return alignText(str, align, width);

        // Divide string near width, OR at '\n'.
        for (size_t i = 0; i < wraps; i++) {

            // Indenting Behavior
            if (!INDENT) { if (tempStr[0] == ' ') tempStr.replace(0, 1, ""); }
            else if (i != 0) tempStr.insert(0, string(INDENT_AMT, ' '));

            // Create substrings at each newline character.
            if (newlines > 0) {
                string shortStr = tempStr.substr(0, width - 2);
                size_t newLinePos = shortStr.find('\n', 1);
                if (newLinePos != string::npos) {
                    string newLineStr = tempStr.replace(newLinePos, 1, " ");
                    cutStr.push_back(newLineStr.substr(0, newLinePos));
                    tempStr.replace(0, newLinePos, "");
                    continue;
                }
            }

            // If it's the last line, avoid substringing.
            if (visibleLength(tempStr) <= width - 2) {
                cutStr.push_back(tempStr);
                break;
            }

            // Seek spaces near end-of-line.
            // MODIFIED TO ADJUST FOR HIDDEN ANSI ESCAPE CHARACTERS
            size_t diff = width - (visibleLength(tempStr.substr(0, width - 2)));
            size_t adj = width + diff - 2;
            size_t pos = tempStr.rfind(' ', (adj - 2));
            if (pos <= tempStr.length()) {
                cutStr.push_back(tempStr.substr(0, pos));
                tempStr.replace(0, pos, "");
            }
            // If no space exists, hyphenate to newline.
            else if (pos == string::npos) {
                cutStr.push_back(tempStr.substr(0, width - 3) + "-");
                tempStr.replace(0, width - 3, "");
            }
        }

        // Combine each element of cutStr[]. Adjust for newline characters.
        string finalStr = "";
        for (size_t i = 0; i < cutStr.size(); i++) {
            if (!cutStr[i].empty()) {
                finalStr += alignText(cutStr[i], align, width);

                // Apply the optional 'fix' parameter (default = '\n')
                if (i < wraps - 1) finalStr += fix;
            }
        }
        return finalStr;
    }

    // Count and return the number of '\n' characters in a string.
    // Also replaces "\\n" with '\n' for proper formatting.
    size_t countNewLineChars(string& str) const {
        size_t numNewLines = 0;
        size_t foundStrNL = string::npos;
        size_t foundCharNL = string::npos;
        while (true) {
            foundStrNL = str.find("\\n", foundStrNL + 1);
            foundCharNL = str.find('\n', foundCharNL + 1);
            if (foundStrNL != string::npos) {
                str.replace(foundStrNL, 2, "\n");
                numNewLines++;
            }
            else if (foundCharNL != string::npos) numNewLines++;
            else break;
        }
        return numNewLines;
    }

    // Replace '\t' characters with spaces using TAB_SIZE.
    void replaceTabChars(string& str) const {
        size_t foundTab = string::npos;
        while (true) {
            foundTab = str.find('\t', foundTab + 1);
            if (foundTab != string::npos) {
                str.replace(foundTab, 2, "");
                str.insert(foundTab, TAB_SIZE, ' ');
            }
            else break;
        }
    }
};


/*    StatusBar Struct    */
// Prints a colorized status/progress bar based on the numbers passed
// I could definitely modularize this more, but I'm happy with it for now.
struct StatusBar {

    string statusBar(int cur, int max, int numBars) const {

        string statusBar;
        string bars = "";

        // Define Colors for each section 
        // I should be able to modularize this fairly easily
        const int NUM_SECTIONS = 3;
        Colors colors[NUM_SECTIONS] = { REDBG, YELLOWBG, GREENBG };
        int SECTION = max / NUM_SECTIONS;

        // Standardize values
        double num = (double(cur) / max) * 100;
        int lvl = static_cast<int>(round((num * .10) * (numBars * 0.10)));
        max = max * 1;

        // Determine color based on percentage
        Colors barColor = colors[NUM_SECTIONS - 1];
        Colors mod1 = BRIGHT;

        // Sub-section currently has no effect, since we're using Background 
        // colors
        for (int i = 0; i < NUM_SECTIONS; i++) {
            if (cur < SECTION * (i + 1)) {
                barColor = colors[i];
                // Determine sub-section (15% higher than current = BRIGHT)
                if (cur + (max / 100 * 15) < SECTION * (i + 1)) mod1 = RESET;
                break;
            }
        }

        // Final formatting
        bars = ColorText().colorize(string(lvl, ' '), barColor, mod1);
        bars = AlignText().alignText(bars, 'L', numBars);
        statusBar = "[" + bars + "]";
        return statusBar;
    }
};

// Allows the control of all Text-Altering classes in one object
struct FormatText : public ColorText, public WrapText, public StatusBar {

    FormatText(int indentAmt = 0) : WrapText(indentAmt) {
        Logger().logTime(3, "FormatText Object Created.");
    }

    // Clears Entire Screen
    void clearScreen() const { cout << "\033[2J\033[1;1H"; }

    // Clears 'n' lines from output
    void clearLines(int n) const {
        for (int i = 0; i < n; i++) cout << "\033[A\033[K";
    }
};

// Global function (instead of using a Macro)
string static align(string str, char align, int W, char padChar = ' ') {
    return FormatText().alignText(str, align, W, padChar);
}
