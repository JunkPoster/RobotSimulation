#include <iostream>
#include <ctime>
#include <string>
using namespace std;

/*******************
*    Date Class    *
*******************/
// Stores the System time into strings. Returns date in MM/DD/YYYY format
class Date {
    time_t currentTime;
    struct tm localTime;
    int m, d, y;
public:
    Date() { setDateVariables(); }
    ~Date() {}
    
    // Convert current system time to usable data (strings)
    void setDateVariables() {
        time(&currentTime);
        localtime_s(&localTime, &currentTime);
        char mon[6], day[6], year[10];
        strftime(mon, 6, "%m", &localTime);
        strftime(day, 6, "%d", &localTime);
        strftime(year, 10, "%Y", &localTime);
    
        // Assign to mon/day/year variables
        m = atoi(mon), d = atoi(day), y = atoi(year); 
    }
    
    // Return current date in MM/DD/YYYY format, WITH leading Zero's
    string getDate() const {
        string str = "";
        
        // Add Leading Zeros to Month & Day, if necessary
        if (m < 10) str += "0";
        str += to_string(m) + "/";
        
        if (d < 10) str += "0";
        str += to_string(d) + "/";
        
        str += to_string(y);
        
        //LOG(4, str);//("Date::getDate() Returned " + str));
        return str;
    }
    /*
    bool ComponentTest() {
        string curDateClass = getDate();
        string extDateClass = Date().getDate();
        
        // Test getDate() & consistency across Date classes
        if (curDateClass != extDateClass) {
            LOGERROR("getDate()", "Date Mismatch!");
            return false;
        }
        
        return true;
    }*/
};