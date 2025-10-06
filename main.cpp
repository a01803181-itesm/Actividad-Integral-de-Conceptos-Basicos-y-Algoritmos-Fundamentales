/*
Descripcion: El siguiente programa ordena los eventos del archivo equipo5.csv por fecha y hora, luego los escribe en un nuevo archivo llamado sorted_equipo5.csv. Finalmente, el programa permite al usuario buscar eventos dentro de un rango de fechas especificado hasta que el usuario decida salir del programa.

Autores:
- Jebrana Yatziri Balvanera Chumacero (A01803133)
- Alexander Mejia Tovar (A01803181)
- Marco Polo Peña Rivera (A01754314)

Fecha: 2023-10-11
*/

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
    char dateOutput[20];
    strftime(dateOutput, 20, "%d-%m-%Y,%T", &e.ts);
    os << dateOutput << "," << e.ip_o << "," << e.port_o << "," 
		<< e.domain_o << "," << e.ip_d << "," << e.port_d << "," 
		<< e.domain_d;
    return os;
}

// Overload of < operator for struct event
// Will determine sorting order of events
// e1 < e2 iff e1.ts < e2.ts
bool operator<(const event &e1, const event &e2) {
    struct tm tm1 = e1.ts;
    struct tm tm2 = e2.ts;
    time_t tmE1 = std::mktime(&tm1);
    time_t tmE2 = std::mktime(&tm2);
    return tmE1 < tmE2;
}

// Function to parse and save the dates from the csv correctly to Event struct
// Cell is the string that contains the date
// CurrentEvent is the Event struct that will be modified
// Returns the modified Event struct
event parseAndSaveDate(std::string cell, event currentEvent) {
    int dateDataIndex = 0;
    int stringIndex = 0;
    std::string dateData{};
    while(dateDataIndex < 3) {
        dateData = "";
        while(stringIndex < cell.size() && cell[stringIndex] != '-' && cell[stringIndex] != '/') {
            dateData.append(cell.substr(stringIndex, 1));
            stringIndex++;
        }
        stringIndex++;
        switch(dateDataIndex) {
            case 0:
                currentEvent.ts.tm_mday = std::stoi(dateData);
                break;
            case 1:
                // Months are 0-based in tm struct
                currentEvent.ts.tm_mon = std::stoi(dateData) - 1;
                break;
            case 2:
                // Years are 1900-based in tm struct
                currentEvent.ts.tm_year = std::stoi(dateData) - 1900;
                break;
            default:
                break;
        }
        dateDataIndex++;
    }
    return currentEvent;
}

// Function to parse and save the time from the csv correctly to Event struct
// Cell is the string that contains the time
// CurrentEvent is the Event struct that will be modified
// Returns the modified Event struct
event parseAndSaveTime(std::string cell, event currentEvent) {
    int timeDataIndex = 0;
    int stringIndex = 0;
    std:string timeData{};

    while(timeDataIndex < 3) {
        timeData = "";
        while(stringIndex < cell.size() && cell[stringIndex] != ':' && cell[stringIndex] != ' ') {
            timeData.append(cell.substr(stringIndex, 1));
            stringIndex++;
        }
        stringIndex++;
        switch(timeDataIndex) {
            case 0:
                // If time is PM, add 12 to the hour. 12 PM is 12, 12 AM is 0
                // Note: npos is a value returned by find() when the substring is not found
                if (cell.find("PM") != std::string::npos && timeData != "12")
                    timeData = std::to_string(std::stoi(timeData) + 12);
                currentEvent.ts.tm_hour = std::stoi(timeData);
                break;
            case 1:
                currentEvent.ts.tm_min = std::stoi(timeData);
                break;
            case 2:
                currentEvent.ts.tm_sec = std::stoi(timeData);
                break;
            default:
                break;
        }
        timeDataIndex++;
    }
    return currentEvent;
}

// Function to parse and save the IP's from the csv correctly to Event struct
// Cell is the string that contains the IP's
// CurrentEvent is the Event struct that will be modified
// Type is the type of IP that will be modified (origin or destination)
// Returns the modified Event struct
event parseAndSaveIP(std::string cell, event currentEvent, std::string type) {
    int portIndex = 0;
    int stringIndex = 0;
    std::string portData{};
    if (type == "origin") {
        if (cell == "-")
            currentEvent.ip_o = {0,0,0,0};
        else {
            while(portIndex < 4) {
                portData = "";
                while (stringIndex < cell.size() && cell[stringIndex] != '.') {
                    portData.append(cell.substr(stringIndex, 1));
                    stringIndex++;
                }
                stringIndex++;
                switch(portIndex) {
                    case 0:
                        currentEvent.ip_o.o1 = std::stoi(portData);
                        break;
                    case 1:
                        currentEvent.ip_o.o2 = std::stoi(portData);
                        break;
                    case 2:
                        currentEvent.ip_o.o3 = std::stoi(portData);
                        break;
                    case 3:
                        currentEvent.ip_o.o4 = std::stoi(portData);
                        break;
                    default:
                        break;
                }
                portIndex++;
            }
        }
    } else {
        if (cell == "-")
            currentEvent.ip_d = {0,0,0,0};
        else {
            while(portIndex < 4) {
                portData = "";
                while (stringIndex < cell.size() && cell[stringIndex] != '.') {
                    portData.append(cell.substr(stringIndex, 1));
                    stringIndex++;
                }
                stringIndex++;
                switch(portIndex) {
                    case 0:
                        currentEvent.ip_d.o1 = std::stoi(portData);
                        break;
                    case 1:
                        currentEvent.ip_d.o2 = std::stoi(portData);
                        break;
                    case 2:
                        currentEvent.ip_d.o3 = std::stoi(portData);
                        break;
                    case 3:
                        currentEvent.ip_d.o4 = std::stoi(portData);
                        break;
                    default:
                        break;
                }
                portIndex++;
            }
        }
    }
    return currentEvent;
}

int main() {
    vector<event> eventVector{};

    // Open the file
    std::ifstream inputFile("equipo5.csv");

    if (!inputFile.is_open()) {
        std::cerr << "Error by opening file" << std::endl;
        return 1;
    }

    std::string line;
    event dynamicEvent{};
    
    int columnIndex = 0;
    std::cout << "Processing data..." << std::endl;

    // Read the csv file line by line
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> eventData;

        // Split the line into cells
        while(std::getline(ss, cell, ',')) {
            switch(columnIndex) {
                case 0:
                    dynamicEvent = parseAndSaveDate(cell, dynamicEvent);
                    break;
                case 1:
                    dynamicEvent = parseAndSaveTime(cell, dynamicEvent);
                    break;
                case 2:
                    dynamicEvent = parseAndSaveIP(cell, dynamicEvent, "origin");
                    break;
                case 3:
                    dynamicEvent.port_o = cell;
                    break;
                case 4:
                    dynamicEvent.domain_o = cell;
                    break;
                case 5:
                    dynamicEvent = parseAndSaveIP(cell, dynamicEvent, "destination");
                    break;
                case 6:
                    dynamicEvent.port_d = cell;
                    break;
                case 7:
                    dynamicEvent.domain_d = cell;
                    break;
                default:
                    break;
            }
            columnIndex++;
            eventData.push_back(cell);
        }
        columnIndex = 0;
        eventVector.push_back(dynamicEvent);
    }
    cout << "Data processing complete!" << std::endl;

    inputFile.close();
    
    cout << "Sorting data..." << std::endl;
    sort(eventVector.begin(), eventVector.end());
    cout << "Sorting complete!" << std::endl;

    std::ofstream outputFile("sorted_equipo5.csv");
    if (!outputFile.is_open()) {
        std::cerr << "Error by opening file" << std::endl;
        return 1;
    }

    // Write the sorted data to the new file
    for (const event& event : eventVector)
        outputFile << event << std::endl;
        
    cout << "Writing ordered data to a new file..." << std::endl;
    
    outputFile.close();
    
    cout << "File writing complete!" << std::endl;

    // Search Menu Loop for events within a date range, stops when user inputs a key different from 1
    string response0 = "-1";
    while (response0 == "-1") {
        int response1 = -1;
        string firstDate{};
        string lastDate{};
        std::tm tmFirst{};
        std::tm tmLast{};
        time_t timeFirst{};
        time_t timeLast{};

        while (response1 == -1) {
            cout << "Insert a date to start the search (format: DD-MM-YYYY): ";
            cin >> firstDate;
            // Check if the date is in the correct format
            std::istringstream ss1(firstDate);
            ss1 >> std::get_time(&tmFirst, "%d-%m-%Y");
            if (ss1.fail())
                cout << "Invalid date format. Please try again." << std::endl;
            else {
                timeFirst = std::mktime(&tmFirst);
                cout << "First date successfully stored" << std::endl;
                response1 = 0;
                int response2 = -1;
                while (response2 == -1) {
                    cout << "Insert a date to end the search (format: DD-MM-YYYY): ";
                    cin >> lastDate;
                    std::istringstream ss2(lastDate);
                    ss2 >> std::get_time(&tmLast, "%d-%m-%Y");
                    if (ss2.fail())
                        cout << "Invalid date format. Please try again." << std::endl;
                    else {
                        timeLast = std::mktime(&tmLast);
                        if (timeFirst > timeLast)
                            cout << "Error: First date is greater than last date" << std::endl;
                        else {
                            response2 = 0;
                            cout << "Last date successfully stored" << std::endl;
                        }
                    }
                }
            }
        }

        // Lambda function to find the first event with a date greater than or equal to the first given date
        auto firstEvent = [&](const event& event) -> bool {
            struct tm tmEvent = event.ts;
            return std::mktime(&tmEvent) >= timeFirst;
        };

        // Lambda function to find the first event with a date greater than the last given date
        auto lastEvent = [&](const event& event) -> bool {
            struct tm tmEvent = event.ts;
            return std::mktime(&tmEvent) > timeLast;
        };

        // Bounds for the range of events within the date range
        // Note: vector<T>::iterator is a type that represents a pointer to an element in a vector of type T
        vector<event>::iterator lowerBound = std::find_if(eventVector.begin(), eventVector.end(), firstEvent);
        vector<event>::iterator upperBound = std::find_if(lowerBound, eventVector.end(), lastEvent);

        // Print the events within the date range
        if (lowerBound == eventVector.end() || lowerBound == upperBound)
            std::cout << "No events found" << std::endl;
        else {
            std::cout << "Events found:" << std::endl;
            int index = 1;
            for (vector<event>::iterator it = lowerBound; it != upperBound; ++it) {
                std::cout << index << ". " << *it << std::endl;
                ++index;
            }
            std::cout << "No more events found within the date range" << std::endl;
        }
        
        cout << "Do you want to search for another date? (1: Yes, Any other key: No): ";
        cin >> response0;
        if (response0 == "1")
            response0 = "-1";
    }
    cout << "Program finished successfully" << std::endl;
    
    return 0;
}