/* KVServer.h */
#ifndef KVSERVER_H
#define KVSERVER_H
#include "ThreadSafeCache.h"
#include "ThreadPool.h"
#include "Connection.h"
#include <string>

struct Config {
    int port = 8080;
    int capacity = 1000;
    size_t numThreads = 4;
};

constexpr Config DEFAULT_CONFIG;

class KVServer {
public:
    // default constructor
    KVServer();

    // overloaded constructor
    KVServer(const Config& cfg);

    // destructor
    ~KVServer() = default;

    // run()
    // main loop of the program:
    // - create tcp socket
    // - bind that socket to a port
    // - listen for connections
    // - start accept loop
    // - wrap 
    void Run();

private:
    Config m_config;
    ThreadSafeCache<std::string, std::string> m_cache;
    ThreadPool m_threadPool;
    int m_serverFd; // file descriptor
};


#endif