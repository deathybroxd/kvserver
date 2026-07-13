/* tests.cpp 
unit testing
*/
#include "KVServer.h" 
#include <iostream>
#include <string>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

int pass = 0, fail = 0;

void check(const std::string& txt, bool condition) {
    if(condition) {
        std::cout << txt << " : " << GREEN << "PASS" << RESET << std::endl;
    } else {
        std::cout << txt << " : " << RED << "FAIL" << RESET << std::endl;
    }
}

int main() {
    KVServer server;
    server.Run();
    
    // TEST 1 - Protocol::Parse
    

    return 0;
}