/* main.cpp*/
#include "KVServer.h"
#include <iostream>

int main() {
    Config cfg;
    cfg.numThreads = 32; // this is just the number of threads on my i9-14900hx
    KVServer server(cfg);
    server.Run();
    return 0;
}