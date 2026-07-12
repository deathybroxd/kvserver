/* Connection.h 
runs the actual commands taken from protocol
*/

#ifndef CONNECTION_H
#define CONNECTION_H

#include "ThreadSafeCache.h"
#include "Protocol.h"
#include <string>

class Connection {
public:
    // constructor
    explicit Connection(int socketFd, ThreadSafeCache<std::string, std::string>& cache);

    ~Connection() = default;

    // handle
    // reads lines, parses, dispatches and writes responses
    void Handle();

private:
    int m_socketFd; // socket file descriptor
    ThreadSafeCache<std::string, std::string>& m_cache;
};



#endif