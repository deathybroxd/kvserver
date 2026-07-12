CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
INCLUDES = -Iinclude

# directories
SRC = src
TEST = tests

# objs
SERVER_OBJS = main.o KVServer.o Connection.o Protocol.o WAL.o
BENCHMARK_OBJS = BenchmarkClient.o Protocol.o
TEST_OBJS = tests.o Protocol.o

# default target - builds everything
all: server benchmark tests

# server binary
server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o server $(SERVER_OBJS)

# benchmark binary
benchmark: $(BENCHMARK_OBJS)
	$(CXX) $(CXXFLAGS) -o benchmark $(BENCHMARK_OBJS)

# tests binary
tests: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o tests $(TEST_OBJS)

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

BenchmarkClient.o: $(SRC)/BenchmarkClient.cpp include/Protocol.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(SRC)/BenchmarkClient.cpp

tests.o: $(TEST)/tests.cpp include/LRUCache.h include/ThreadSafeCache.h include/Protocol.h include/WAL.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(TEST)/tests.cpp

val-server:
	valgrind ./server

val-benchmark:
	valgrind ./benchmark

val-tests:
	valgrind ./tests