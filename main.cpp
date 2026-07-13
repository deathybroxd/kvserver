/* main.cpp*/
#include "KVServer.h"
#include <iostream>

int main() {
    KVServer server;
    server.Run();
    std::cout << "Server running... Ctrl + C to exit" << std::endl;
    return 0;
}