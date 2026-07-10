/* Connection.cpp */
#include "Connection.h"
#include <iostream>
#include <sys/socket.h>

// constructor
Connection::Connection(int socketFd, ThreadSafeCache<std::string, std::string>& cache) : 
m_socketFd(socketFd), m_cache(cache) {}

void Connection::Handle() {
    while() {
        
    }
}

