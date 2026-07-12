/* WAL.cpp
Write Ahead Log
*/
#include "WAL.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// constructor
WAL::WAL(const std::string& filePath) : 
m_filePath(filePath), m_file(m_filePath, std::ios::app) {} // append mode

// Append
    // appends commands with a timestamp to WAL
void WAL::Append(const std::string& cmd, const std::string& key, const std::string& value) {
    if(m_file.is_open()) {
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t now_c = system_clock::to_time_t(now);
        
        // formatting
        std::tm tm_buf;
        localtime_r(&now_c, &tm_buf); // from ctime (c header)
        std::stringstream timestamp;
        timestamp << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        m_file << timestamp.str() << " | " << cmd << " " << key << " " << value << std::endl;
    } else {
        std::cerr << "Error: " << m_filePath << " could not be opened" << std::endl;
        return;
    }
}

// Replay
// called at server startup - reads log file and puts everything back into cache
void WAL::Replay(ThreadSafeCache<std::string, std::string>& cache) {
    std::ifstream readFile(m_filePath);
    if(readFile.is_open()) {
        std::string line, cmd, key, value;
        while(std::getline(readFile, line)) {
            if(!ParseLine(line, cmd, key, value)) {
                continue;
            }

            if(cmd == "SET") {
                cache.Set(key, value);
            } else if (cmd == "DEL") {
                cache.Remove(key);
            }
            // other commands dont need to be run on the cache for replay
        }
    } else {
        std::cerr << "Error: " << m_filePath << " could not be opened" << std::endl;
        return;
    }
}

///// PRIVATE HELPERS /////

// ParseLine
// helper for replay to parse lines from the file
bool WAL::ParseLine(const std::string& line, std::string& cmd, std::string& key, std::string& value) {
    size_t pos = line.find('|');
    if(pos == std::string::npos) {
        return false;
    }

    std::istringstream iss(line.substr(pos + 1)); // + 1 for space
    iss >> cmd;

    if(cmd == "SET") {
        return (bool)(iss >> key >> value); // explicit cast the iss
    } else if(cmd == "DEL") {
        return (bool)(iss >> key);
    }
    return false;
}