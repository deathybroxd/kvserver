/* KVServer.cpp */
#include "KVServer.h"
#include "Connection.h"
#include <iostream>
#include <sys/socket.h> // linux only libraries
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

// default constructor
KVServer::KVServer() : 
m_config(DEFAULT_CONFIG), m_cache(m_config.capacity), m_threadPool(m_config.numThreads), m_wal(m_config.logFile) {}

// overloaded constructor
KVServer::KVServer(const Config& cfg) : 
m_config(cfg), m_cache(m_config.capacity), m_threadPool(m_config.numThreads), m_wal(m_config.logFile) {}

// run()
// main loop of the program:
// - create tcp socket
// - bind that socket to a port
// - listen for connections
// - start accept loop
// - wrap 
void KVServer::Run() {
    // establish socket
    m_serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if(m_serverFd == -1) {
        std::cerr << "Error: socket could not be created" << std::endl;
        return;
    }

    // set socket options
    int opt = 1; // enable option val
    setsockopt(m_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // establish address
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(m_config.port);
    address.sin_addr.s_addr = INADDR_ANY;

    // bind to socket and listen
    bind(m_serverFd, (sockaddr*)&address, sizeof(address));
    listen(m_serverFd, 128);

    m_wal.Replay(m_cache);
    // accept loop
    while(true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(m_serverFd, (sockaddr*)&clientAddr, &clientLen);
        if(clientFd == -1) {
            continue; // skip to next iteration if clientFd not found
        }

        // send to threadpool
        m_threadPool.Submit([clientFd, this]() {
            Connection conn(clientFd, m_cache, m_wal);
            conn.Handle();
        });
    }
}   
