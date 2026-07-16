CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
INCLUDES = -Iinclude

# directories
SRC = src
TEST = tests

# objs
SERVER_OBJS = main.o KVServer.o Connection.o Protocol.o WAL.o ThreadPool.o
BENCHMARK_OBJS = BenchmarkClient.o Protocol.o
TEST_OBJS = KVServerTests.o Protocol.o WAL.o

# default target - builds everything
all: server benchmark run_tests

# server binary
server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o server $(SERVER_OBJS)

# benchmark binary
benchmark: $(BENCHMARK_OBJS)
	$(CXX) $(CXXFLAGS) -o benchmark $(BENCHMARK_OBJS)

# tests binary
KVServerTests: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o KVServerTests $(TEST_OBJS)

# compile rules
main.o: main.cpp include/KVServer.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cpp

KVServer.o: $(SRC)/KVServer.cpp include/KVServer.h include/ThreadSafeCache.h include/ThreadPool.h include/Connection.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC)/KVServer.cpp

Connection.o: $(SRC)/Connection.cpp include/Connection.h include/ThreadSafeCache.h include/Protocol.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC)/Connection.cpp

Protocol.o: $(SRC)/Protocol.cpp include/Protocol.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC)/Protocol.cpp

WAL.o: $(SRC)/WAL.cpp include/WAL.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC)/WAL.cpp

ThreadPool.o: $(SRC)/ThreadPool.cpp include/ThreadPool.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC)/ThreadPool.cpp

BenchmarkClient.o: $(SRC)/BenchmarkClient.cpp include/Protocol.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC)/BenchmarkClient.cpp

KVServerTests.o: $(TEST)/KVServerTests.cpp include/ThreadSafeCache.h include/Protocol.h include/WAL.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(TEST)/KVServerTests.cpp

clean:
	rm -f *.o server benchmark run_tests

run-tests:
	valgrind ./KVServerTests

run-benchmark:
	./benchmark --host 127.0.0.1 --port 8080 --threads 32 --requests 1000

val-benchmark:
	valgrind ./benchmark --host 127.0.0.1 --port 8080 --threads 32 --requests 1000

start-server:
	./server

.PHONY: all clean start-server run-benchmark run-tests