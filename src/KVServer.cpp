/* KVServer.cpp */
#include "KVServer.h"
// NOTE: m_serverFd is default intialized to -1 to signal no socket has been created yet

// default constructor
KVServer::KVServer() : 
m_config(DEFAULT_CONFIG), m_cache(m_config.capacity), m_threadPool(m_config.numThreads), m_serverFd(-1) {}

// overloaded constructor
KVServer::KVServer(const Config& cfg) : 
m_config(cfg), m_cache(cfg.capacity), m_threadPool(cfg.numThreads), m_serverFd(-1) {}

// run()
// main loop of the program:
// - create tcp socket
// - bind that socket to a port
// - listen for connections
// - start accept loop
// - wrap 
void Run() {


}
