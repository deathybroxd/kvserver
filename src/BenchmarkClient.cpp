/* BenchmarkClient.cpp
- separate benchmark main
- opens N concurrent connections to the server, fires a bunch of requests on each connection,
measures how long it takes, and reports throughput (requests/sec) and average ping in ms
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include <string>

// example run:
// 0           1      2         3      4    5         6  7          8
// ./benchmark --host 127.0.0.1 --port 8080 --threads 10 --requests 1000
int main(int argc, char* argv[]) {
    if(argc != 9) {
        std::cerr << "ERROR: Benchmark usage: ./benchmark --host <host> --port <port> --threads <threads> --requests <requests>" << std::endl;
        return 1;
    }

    char* host = argv[2];
    int port = std::stoi(argv[4]);
    int numThreads = std::stoi(argv[6]);
    int numRequests = std::stoi(argv[8]);

    // create server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(host);

    // begin benchmark
    std::cout << "Starting benchmark... | Host: " << host << " | Port: " << port << " | Threads: " << numThreads << " | Requests: " << numRequests << std::endl;
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    std::vector<std::thread> threads;
    for(int i = 0; i < numThreads; ++i) {
        // lambda for each thread to create its own socket
        threads.emplace_back([numRequests, &serverAddr]() {
            int socketFd = socket(AF_INET, SOCK_STREAM, 0);
            if(socketFd == -1) {
                std::cerr << "Error: could not create socket" << std::endl;
                return;
            }

            if(connect(socketFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
                std::cerr << "Error: could not connect to " << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port) << std::endl;
                close(socketFd);
                return;
            }

            for(int j = 0; j < numRequests; ++j) {
                // send each request to the socket
                std::string request = "SET key_" + std::to_string(j) + " value_" + std::to_string(j) + "\n";
                send(socketFd, request.c_str(), request.size(), 0);

                // receive incoming data from socket and put it into the buffer
                char buf[1024];
                recv(socketFd, buf, sizeof(buf), 0);
            }

            close(socketFd);
        });
    }   

    // finish execution
    for(auto& t : threads) {
        t.join();
    }

    // report results
    auto end = high_resolution_clock::now(); // end benchmark
    int totalRequests = numThreads * numRequests;
    double ms = duration<double, std::milli>(end - start).count();
    double throughput = (totalRequests / ms) * 1000.0;
    std::cout << "Benchmark complete: " << totalRequests << " requests in " << ms << "ms | " << throughput << " requests/sec" << std::endl;
    return 0;
}