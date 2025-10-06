#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <format>
#include <iomanip>

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
struct Event {
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
std::ostream& operator<<(std::ostream &os, const Event &e) {
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
bool operator<(const Event &e1, const Event &e2) {
    struct tm tm1 = e1.ts;
    struct tm tm2 = e2.ts;
    time_t tmE1 = std::mktime(&tm1);
    time_t tmE2 = std::mktime(&tm2);
    return tmE1 < tmE2;
}

Event parseAndSaveDate(std::string cell, Event currentEvent) {
    int date_data_index = 0;
    int string_index = 0;
    std::string date_data{};
    //cout << "Cell: " << cell << "\n";
    while(date_data_index < 3) {
        date_data = "";
        while(string_index < cell.size() && (cell[string_index] != '-' && cell[string_index] != '/')) {
            date_data.append(cell.substr(string_index, 1));
            string_index++;
        }
        string_index++;
        //cout << "Date data: " << date_data << "\n";
        switch(date_data_index) {
            case 0:
                currentEvent.ts.tm_mday = std::stoi(date_data);
                break;
            case 1:
                currentEvent.ts.tm_mon = std::stoi(date_data) - 1;
                break;
            case 2:
                currentEvent.ts.tm_year = std::stoi(date_data) - 1900;
                break;
            default:
                break;
        }
        date_data_index++;
    }
    /*
    cout << "ALERT. LOG IN PROGRESS" << "\n";
    cout << "Day: " << currentEvent.ts.tm_mday << "\n";
    cout << "Month: " << currentEvent.ts.tm_mon << "\n";
    cout << "Year: " << currentEvent.ts.tm_year << "\n";
    char date_output[10];
    strftime(date_output, 10, "%d-%m-%Y", &currentEvent.ts);
    cout << "Date: " << date_output << "\n";
    */
    return currentEvent;
}

Event parseAndSaveTime(std::string cell, Event currentEvent) {
    int time_data_index = 0;
    int string_index = 0;
    std:string time_data{};

    while(time_data_index < 3) {
        time_data = "";
        while(string_index < cell.size() && (cell[string_index] != ':' && cell[string_index] != ' ')) {
            time_data.append(cell.substr(string_index, 1));
            string_index++;
        }
        string_index++;
        //cout << "Time data: " << time_data << "\n";
        switch(time_data_index) {
            case 0:
                if (cell.find("PM") != std::string::npos && time_data != "12")
                    time_data = std::to_string(std::stoi(time_data) + 12);
                currentEvent.ts.tm_hour = std::stoi(time_data);
                break;
            case 1:
                currentEvent.ts.tm_min = std::stoi(time_data);
                break;
            case 2:
                currentEvent.ts.tm_sec = std::stoi(time_data);
                break;
            default:
                break;
        }
        time_data_index++;
    }
    /*
    cout << "ALERT. LOG IN PROGRESS" << "\n";
    cout << "Hour: " << currentEvent.ts.tm_hour << "\n";
    cout << "Minute: " << currentEvent.ts.tm_min << "\n";
    cout << "Second: " << currentEvent.ts.tm_sec << "\n";
    */
    return currentEvent;
}

Event parseAndSaveIP(std::string cell, Event currentEvent, std::string type) {
    int port_index = 0;
    int string_index = 0;
    std::string port_data{};
    if (type == "origin") {
        if (cell == "-")
            currentEvent.ip_o = {0,0,0,0};
        else {
            while(port_index < 4) {
                port_data = "";
                while (string_index < cell.size() && cell[string_index] != '.') {
                    port_data.append(cell.substr(string_index, 1));
                    string_index++;
                }
                string_index++;
                switch(port_index) {
                    case 0:
                        currentEvent.ip_o.o1 = std::stoi(port_data);
                        break;
                    case 1:
                        currentEvent.ip_o.o2 = std::stoi(port_data);
                        break;
                    case 2:
                        currentEvent.ip_o.o3 = std::stoi(port_data);
                        break;
                    case 3:
                        currentEvent.ip_o.o4 = std::stoi(port_data);
                        break;
                    default:
                        break;
                }
                port_index++;
            }
        }
        /*
        cout << "ALERT. LOG IN PROGRESS" << "\n";
        cout << "IP: " << currentEvent.ip_o << "\n";
        */
    } else {
        if (cell == "-")
            currentEvent.ip_d = {0,0,0,0};
        else {
            while(port_index < 4) {
                port_data = "";
                while (string_index < cell.size() && cell[string_index] != '.') {
                    port_data.append(cell.substr(string_index, 1));
                    string_index++;
                }
                string_index++;
                switch(port_index) {
                    case 0:
                        currentEvent.ip_d.o1 = std::stoi(port_data);
                        break;
                    case 1:
                        currentEvent.ip_d.o2 = std::stoi(port_data);
                        break;
                    case 2:
                        currentEvent.ip_d.o3 = std::stoi(port_data);
                        break;
                    case 3:
                        currentEvent.ip_d.o4 = std::stoi(port_data);
                        break;
                    default:
                        break;
                }
                port_index++;
            }
        }
        /*
        cout << "ALERT. LOG IN PROGRESS" << "\n";
        cout << "IP: " << currentEvent.ip_d << "\n";
        */
    }
    return currentEvent;
}


int main() {
    // *** Usage example of struct e (delete in final version) ***
    Event e{}; // Creates a new event
    
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
    vector<Event> v{};

    std::ifstream file("equipo5.csv");

    if (!file.is_open()) {
        std::cerr << "Error by opening file" << std::endl;
        return 1;
    }

    std::string line;
    //std::getline(file, line);
    Event dynamic_event{};
    
    int column_index = 0;
    std::cout << "Processing data..." << std::endl;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> event_data;
        
        while(std::getline(ss, cell, ',')) {
            switch(column_index) {
                case 0:
                    dynamic_event = parseAndSaveDate(cell, dynamic_event);
                    break;
                case 1:
                    dynamic_event = parseAndSaveTime(cell, dynamic_event);
                    break;
                case 2:
                    dynamic_event = parseAndSaveIP(cell, dynamic_event, "origin");
                    break;
                case 3:
                    dynamic_event.port_o = cell;
                    break;
                case 4:
                    dynamic_event.domain_o = cell;
                    break;
                case 5:
                    dynamic_event = parseAndSaveIP(cell, dynamic_event, "destination");
                    break;
                case 6:
                    dynamic_event.port_d = cell;
                    break;
                case 7:
                    dynamic_event.domain_d = cell;
                    break;
                default:
                    break;
            }
            column_index++;
            event_data.push_back(cell);
        }
        column_index = 0;
        v.push_back(dynamic_event);
        //cout << "Event: " << dynamic_event << "\n";
    }

    cout << "Data processing complete!" << "\n";

    cout << "Sorting data..." << "\n";
    sort(v.begin(), v.end());
    cout << "Sorting complete!" << "\n";

    std::ofstream output_file("sorted_equipo5.csv");
    if (!output_file.is_open()) {
        std::cerr << "Error by opening file" << std::endl;
        return 1;
    }
    for (const auto& event : v) {
        output_file << event << "\n";
    }
    cout << "Writing ordered data to a new file..." << "\n";
    
    output_file.close();
    cout << "File writing complete!" << "\n";

    string response0 = "-1";
    while (response0 == "-1") {
        int response1 = -1;
        string first_date{};
        string last_date{};
        std::tm tm_first{};
        std::tm tm_last{};
        time_t time_first{};
        time_t time_last{};

        while (response1 == -1) {
            cout << "Insert a date to start the search (format: DD-MM-YYYY): ";
            cin >> first_date;
            std::istringstream ss1(first_date);
            ss1 >> std::get_time(&tm_first, "%d-%m-%Y");
            if (ss1.fail())
                cout << "Invalid date format. Please try again." << "\n";
            else {
                time_first = std::mktime(&tm_first);
                cout << "First date successfully stored" << "\n";
                response1 = 0;
                int response2 = -1;
                while (response2 == -1) {
                    cout << "Insert a date to end the search (format: DD-MM-YYYY): ";
                    cin >> last_date;
                    std::istringstream ss2(last_date);
                    ss2 >> std::get_time(&tm_last, "%d-%m-%Y");
                    if (ss2.fail())
                        cout << "Invalid date format. Please try again." << "\n";
                    else {
                        time_last = std::mktime(&tm_last);
                        if (time_first > time_last)
                            cout << "Error: First date is greater than last date" << "\n";
                        else {
                            response2 = 0;
                            cout << "Last date successfully stored" << "\n";
                        }
                    }
                }
            }
        }

        auto first_event = [&](const Event& event) -> bool {
            struct tm tm_event = event.ts;
            return std::mktime(&tm_event) >= time_first;
        };
        auto last_event = [&](const Event& event) -> bool {
            struct tm tm_event = event.ts;
            return std::mktime(&tm_event) > time_last;
        };

        auto lower_bound = std::find_if(v.begin(), v.end(), first_event);
        auto upper_bound = std::find_if(lower_bound, v.end(), last_event);

        if (lower_bound == v.end() || lower_bound == upper_bound) {
            std::cout << "No events found\n";
        } else {
            std::cout << "Events found:\n";
            for (auto it = lower_bound; it != upper_bound; ++it) {
                std::cout << *it << "\n";
            }
        }
        
        cout << "Do you want to search for another date? (1: Yes, Any other key: No): ";
        cin >> response0;
        if (response0 == "1")
            response0 = "-1";
    }

    // Read file
        // For each line, create an event with corresponding values
        // Store event in vector
    // Sort vector (use https://cplusplus.com/reference/algorithm/sort/)
    // Write out ordered events to new file
    // Prompt user for date
        // Define lambda or functor to compare event with provided dates (event's date should be >= than date
        // Print to console all events starting from date (inclusive)
		// using https://cplusplus.com/reference/algorithm/find_if/)
    
    file.close();
    
    return 0;
}