/* WAL.h 
Write Ahead Log
safety measure to saved cached data if the server crashes
*/

#ifndef WAL_H
#define WAL_H

#include "ThreadSafeCache.h"
#include <string>
#include <fstream>

class WAL {
public:
    // constructor
    explicit WAL(const std::string& filePath);

    // destructor
    ~WAL() = default;

    // Append
    // appends commands with a timestamp to WAL
    void Append(const std::string& cmd, const std::string& key, const std::string& value);

    // Replay
    // called at server startup - reads log file and puts everything back into cache
    void Replay(ThreadSafeCache<std::string, std::string>& cache);

private:
    // ParseLine
    // helper for replay to parse lines from the file
    bool ParseLine(const std::string& line, std::string& cmd, std::string& key, std::string& value);

    std::string m_filePath;
    std::ofstream m_file;
};
#endif