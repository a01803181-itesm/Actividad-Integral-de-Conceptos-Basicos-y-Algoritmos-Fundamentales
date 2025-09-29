#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>

using std::cin;
using std::cout;
using std::string;
using std::vector;

// Struct representing an IP address
struct ip {
    int o1;
    int o2;
    int o3;
    int o4;
};

// Struct representing a log event (connection between two nodes)
// ts: Date-time of the event as ctime's tm (check docs: https://cplusplus.com/reference/ctime/tm/)
// ip_o: IP of the origin node
// port_o: Port of the origin node
// domain_o: Domain of the origin node
// ip_d: IP of the destination node
// port_d: Port of the destination node
// domain_d: Domain of the destination node
struct event {
  struct std::tm ts;
  struct ip ip_o;
  string port_o;
  string domain_o;
  struct ip ip_d;
  string port_d;
  string domain_d;
};

// Overload of << operator for struct ip
// If IP starts with 0 (o1 == 0) prints '-' instead
std::ostream& operator<<(std::ostream &os, const ip &i) {
    if(i.o1 == 0) {
        os << "-";
    } else {
       os << i.o1 << "." << i.o2 << "." << i.o3 << "." << i.o4;   
    }
    return os;
}

// Overload of << operator for struct event
// Prints event using same format as log
std::ostream& operator<<(std::ostream &os, const event &e) {
    char date_output[20];
    strftime(date_output, 20, "%d-%m-%Y,%T", &e.ts);
    os << date_output << "," << e.ip_o << "," << e.port_o << "," 
		<< e.domain_o << "," << e.ip_d << "," << e.port_d << "," 
		<< e.domain_d;
    return os;
}

// Overload of < operator for struct event
// Will determine sorting order of events
// e1 < e2 iff e1.ts < e2.ts
bool operator<(const event &e1, const event &e2) {
    return false;
}

int main() {
    // *** Usage example of struct e (delete in final version) ***
    event e{}; // Creates a new event
    
    // Sets date of event
    // Example date: 09/09/2024, 1:15:10 pm
    e.ts.tm_year = 2024 - 1900; // years since 1900
    e.ts.tm_mon = 8; // months since january
    e.ts.tm_mday = 9; // day of the month
    e.ts.tm_hour = 13; // hours since midnight
    e.ts.tm_min = 15; // minutes after the hour
    e.ts.tm_sec = 10; // seconds after the minute

    // Sets rest of attributes
    e.ip_o = {10,48,124,211}; // Sets ip_o as new ip
    e.ip_d = {0,1,1,1}; // Will print as '-'
    e.port_o = "-";
    e.port_d = "100";
    e.domain_o = "john.reto.com";
    e.domain_d = "google.com";

    cout << e << "\n";
    
    // *** TODO ***
    vector<event> v{};

    // Read file
        // For each line, create an event with corresponding values
        // Store event in vector
    // Sort vector (use https://cplusplus.com/reference/algorithm/sort/)
    // Write out ordered events to new file
    // Prompt user for date
        // Define lambda or functor to compare event with provided dates (event's date should be >= than date
        // Print to console all events starting from date (inclusive)
		// using https://cplusplus.com/reference/algorithm/find_if/)

    return 0;
}