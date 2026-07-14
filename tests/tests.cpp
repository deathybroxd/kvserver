/* tests.cpp 
unit testing - does NOT test the actual server - BenchmarkClient does
*/
#include "Protocol.h"
#include "ThreadSafeCache.h"
#include "WAL.h"
#include <iostream>
#include <string>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

int pass = 0, fail = 0;

void check(const std::string& txt, bool condition) {
    if(condition) {
        std::cout << txt << " : " << GREEN << "PASS" << RESET << std::endl;
        pass++;
    } else {
        std::cout << txt << " : " << RED << "FAIL" << RESET << std::endl;
        fail++;
    }
}

void line() {
    std::cout << std::string(50, '-') << std::endl;
}

int main() {
    // TEST 1 - Protocol::Parse
    line();
    std::cout << "TEST 1: Protocol::Parse " << std::endl;
    Protocol::Request req1 = Protocol::Parse("soidfughnuhg");
    check("Protocol::Parse(\"soidfughnuhg\") should be an error", req1.cmd == Protocol::Command::ERROR);
    line();

    Protocol::Request req2 = Protocol::Parse("PING");
    check("Protocol::Parse(\"PING\") should give no error ", req2.cmd == Protocol::Command::PING);
    line();

    Protocol::Request req3 = Protocol::Parse("GET");
    check("Protocol::Parse(\"GET\") should give an error for no key ", (req3.cmd == Protocol::Command::GET && req3.key == ""));
    line();

    Protocol::Request req4 = Protocol::Parse("SET");
    check("Protocol::Parse(\"SET\") should give an error for no key ", (req4.cmd == Protocol::Command::SET && req4.key == ""));
    line();

    Protocol::Request req5 = Protocol::Parse("SET key");
    check("Protocol::Parse(\"SET key\") should give an error for no value ", (req5.cmd == Protocol::Command::SET && req5.key == "key"));
    line();

    Protocol::Request req6 = Protocol::Parse("DEL");
    check("Protocol::Parse(\"DEL\") should give an error for no key ", (req6.cmd == Protocol::Command::SET && req6.key == ""));
    line();

    Protocol::Request req7 = Protocol::Parse("GET key");
    check("Protocol::Parse(\"GET key\") should give no error ", (req7.cmd == Protocol::Command::GET && req7.key == "key"));
    line();



    // TEST 2 - Protocol::Format
    std::cout << "RESULTS: " << std::endl;
    std::cout << GREEN << "PASS : " << RESET << pass << std::endl;
    std::cout << RED << "FAIL : " << RESET << fail << std::endl;





    // RESULTS
    std::cout << "RESULTS: " << std::endl;
    std::cout << GREEN << "PASS : " << RESET << pass << std::endl;
    std::cout << RED << "FAIL : " << RESET << fail << std::endl;
    line();


    return 0;
}