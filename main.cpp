/* main.cpp*/
#include "KVServer.h"
#include <iostream>
#include <thread>

int main() {
    unsigned int userThreads = std::thread::hardware_concurrency();
    Config cfg;
    cfg.numThreads = userThreads;
    KVServer server(cfg);
    server.Run();
    return 0;
}