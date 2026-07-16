/* KVServerTests.cpp 
unit testing - does NOT test the actual server - BenchmarkClient does
*/
#include "Protocol.h"
#include "ThreadSafeCache.h"
#include "WAL.h"
#include <iostream>
#include <string>
#include <array>
#include <cstddef>
#include <filesystem>

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
    line();
    std::cout << std::endl;

    line();
    Protocol::Request req1 = Protocol::Parse("soidfughnuhg");
    check("Protocol::Request req1 = Protocol::Parse(\"soidfughnuhg\") should be an error", req1.cmd == Protocol::Command::ERROR);
    line();

    Protocol::Request req2 = Protocol::Parse("PING");
    check("Protocol::Request req2 = Protocol::Parse(\"PING\") should give no error ", req2.cmd == Protocol::Command::PING);
    line();

    Protocol::Request req3 = Protocol::Parse("GET");
    check("Protocol::Request req3 = Protocol::Parse(\"GET\") should give an error for no key ", req3.cmd == Protocol::Command::ERROR);
    line();

    Protocol::Request req4 = Protocol::Parse("SET");
    check("Protocol::Request req4 = Protocol::Parse(\"SET\") should give an error for no key ", req4.cmd == Protocol::Command::ERROR);
    line();

    Protocol::Request req5 = Protocol::Parse("SET key");
    check("Protocol::Request req5 = Protocol::Parse(\"SET key\") should give an error for no value ", req5.cmd == Protocol::Command::ERROR);
    line();

    Protocol::Request req6 = Protocol::Parse("DEL");
    check("Protocol::Request req6 = Protocol::Parse(\"DEL\") should give an error for no key ", req6.cmd == Protocol::Command::ERROR);
    line();

    Protocol::Request req7 = Protocol::Parse("GET key");
    check("Protocol::Request req7 = Protocol::Parse(\"GET key\") should give no error and have a key", (req7.cmd == Protocol::Command::GET && req7.key == "key"));
    line();

    Protocol::Request req8 = Protocol::Parse("SET key value");
    check("Protocol::Request req8 = Protocol::Parse(\"SET key value\") should give no error and have a key and value", (req8.cmd == Protocol::Command::SET && req8.key == "key" && req8.value == "value"));
    line();

    Protocol::Request req9 = Protocol::Parse("DEL key");
    check("Protocol::Request req9 = Protocol::Parse(\"DEL key\") should give no error and have a key", (req9.cmd == Protocol::Command::DEL && req9.key == "key"));
    line();

    // TEST 2 - Protocol::Format
    line();
    std::cout << "TEST 2: Protocol::Format " << std::endl;
    line();
    std::cout << std::endl;

    line();
    std::cout << "Using the 9 requests from before, we check their formatting: " << std::endl;
    std::string str1 = Protocol::Format(req1.cmd, std::nullopt, req1.errorMsg);
    check("req1 should give: ERROR: Unknown command soidfughnuhg", str1 == "ERROR: Unknown command soidfughnuhg");
    line();

    std::string str2 = Protocol::Format(req2.cmd, std::nullopt, req2.errorMsg);
    check("req2 should give: PONG", str2 == "PONG");
    line();

    std::string str3 = Protocol::Format(req3.cmd, std::nullopt, req3.errorMsg);
    check("req3 should give: ERROR: GET requires <key>", str3 == "ERROR: GET requires <key>");
    line();
    
    std::string str4 = Protocol::Format(req4.cmd, std::nullopt, req4.errorMsg);
    check("req4 should give: ERROR: SET requires <key> <value>", str4 == "ERROR: SET requires <key> <value>");
    line();

    std::string str5 = Protocol::Format(req5.cmd, std::nullopt, req5.errorMsg);
    check("req5 should give: ERROR: SET requires <key> <value>", str5 == "ERROR: SET requires <key> <value>");
    line();

    std::string str6 = Protocol::Format(req6.cmd, std::nullopt, req6.errorMsg);
    check("req6 should give: ERROR: DEL requires <key>", str6 == "ERROR: DEL requires <key>");
    line();

    std::string str7 = Protocol::Format(req7.cmd, "value", req7.errorMsg);
    check("req7 should give: value", str7 == "value");
    line();

    std::string str8 = Protocol::Format(req8.cmd, std::nullopt, req8.errorMsg);
    check("req8 should give: OK", str8 == "OK");
    line();

    std::string str9 = Protocol::Format(req9.cmd, "value", req9.errorMsg);
    check("req9 should give: DELETED", str9 == "DELETED");
    line();

    line();
    std::cout << "TEST 3: ThreadSafeCache TESTS" << std::endl;
    line();
    std::cout << std::endl;

    // NOTE: these tests are ripped straight from my LRUCache project
    // copy and move operations are not supported by ThreadSafeCache, so they are removed from this testing
    // ---------------- TEST 3a: Constructor / Capacity ----------------
    line();
    std::cout << "TEST 3a - Constructor and Capacity" << std::endl;
    {
        ThreadSafeCache<int, int> normal(3);
        check("normal capacity stored correctly", normal.GetCapacity() == 3);

        ThreadSafeCache<int, int> zeroCap(0);
        check("capacity 0 throws and clamps to default (1)", zeroCap.GetCapacity() == 1);

        ThreadSafeCache<int, int> negCap(-5);
        check("negative capacity throws and clamps to default (1)", negCap.GetCapacity() == 1);

        ThreadSafeCache<int, int> fresh(3);
        check("new cache is empty", fresh.IsEmpty() == true);
        check("new cache has size 0", fresh.GetSize() == 0);
    }

    // ---------------- TEST 3b: Basic Push / Get ----------------
    line();
    std::cout << "TEST 3b - Basic Push and Get" << std::endl;
    {
        ThreadSafeCache<std::string, int> cache(3);
        cache.Set("a", 1);
        check("size is 1 after first push", cache.GetSize() == 1);
        check("cache not empty after push", cache.IsEmpty() == false);

        auto val = cache.Get("a");
        check("get existing key returns value", val.has_value() && val.value() == 1);

        auto missing = cache.Get("z");
        check("get missing key returns nullopt", missing.has_value() == false);

        cache.Set("a", 99);
        auto updated = cache.Get("a");
        check("push on existing key updates value", updated.has_value() && updated.value() == 99);
        check("push on existing key does not grow size", cache.GetSize() == 1);
    }

    // ---------------- TEST 3c: Eviction Order ----------------
    line();
    std::cout << "TEST 3c - Eviction Order (LRU correctness)" << std::endl;
    {
        ThreadSafeCache<std::string, int> cache(2);
        cache.Set("a", 1);
        cache.Set("b", 2);
        check("size is 2 at capacity", cache.GetSize() == 2);

        // touching "a" should promote it, making "b" the LRU
        auto val = cache.Get("a");
        check("get promotes accessed key", val.has_value() && val.value() == 1);

        cache.Set("c", 3); // should evict "b", not "a"
        check("size stays at capacity after eviction", cache.GetSize() == 2);
        check("LRU key (b) was evicted", cache.Get("b").has_value() == false);
        check("recently used key (a) survived eviction", cache.Get("a").has_value() == true);
        check("newly inserted key (c) is present", cache.Get("c").has_value() == true);
    }

    // ---------------- TEST 3d: Capacity 1 edge case ----------------
    line();
    std::cout << "TEST 3d - Capacity 1 Edge Case" << std::endl;
    {
        ThreadSafeCache<std::string, int> cache(1);
        cache.Set("a", 1);
        cache.Set("b", 2); // should evict "a" immediately
        check("size stays at 1", cache.GetSize() == 1);
        check("a was evicted", cache.Get("a").has_value() == false);
        check("b is present", cache.Get("b").has_value() == true);
    }

    // ---------------- TEST 3e: Remove ----------------
    line();
    std::cout << "TEST 3e - Remove" << std::endl;
    {
        ThreadSafeCache<std::string, int> cache(3);
        cache.Set("a", 1);
        cache.Set("b", 2);
        cache.Set("c", 3);

        auto removedMiddle = cache.Remove("b");
        check("remove middle node returns correct value", removedMiddle.has_value() && removedMiddle.value() == 2);
        check("size decreases after remove", cache.GetSize() == 2);
        check("removed key no longer found", cache.Get("b").has_value() == false);
        check("remaining keys still accessible (a)", cache.Get("a").has_value() == true);
        check("remaining keys still accessible (c)", cache.Get("c").has_value() == true);

        auto removeMissing = cache.Remove("z");
        check("remove on missing key returns nullopt", removeMissing.has_value() == false);

        // remove down to a single node, then remove that node too
        cache.Remove("a"); // whichever order it's in, drain to one
        cache.Remove("c");
        check("cache empty after removing all nodes", cache.IsEmpty() == true);
        check("size is 0 after removing all nodes", cache.GetSize() == 0);
    }

    // ---------------- TEST 3f: Clear ----------------
    line();
    std::cout << "TEST 3f - Clear" << std::endl;
    {
        ThreadSafeCache<std::string, int> cache(5);
        cache.Set("a", 1);
        cache.Set("b", 2);
        cache.Set("c", 3);

        cache.Clear();
        check("cache is empty after Clear", cache.IsEmpty() == true);
        check("size is 0 after Clear", cache.GetSize() == 0);
        check("capacity unchanged after Clear", cache.GetCapacity() == 5);
        check("values gone after Clear", cache.Get("a").has_value() == false);

        // make sure cache still works after Clear
        cache.Set("x", 10);
        check("cache usable after Clear", cache.Get("x").has_value() && cache.Get("x").value() == 10);
    }

    // test 4 - WAL
    line();
    std::cout << "TEST 4: WAL TESTS" << std::endl;
    std::cout << "Using previous 9 requests to append, + 1 more " << std::endl;
    line();
    std::cout << std::endl;

    const std::string FILENAME = "testWAL.log";
    try {
        std::filesystem::remove(FILENAME); // delete old testWAL if it exists, if not, make it
    } catch (const std::filesystem::filesystem_error&) {}

    WAL wal1(FILENAME);
    wal1.Append("soidfughnuhg", req1.key, req1.value);
    wal1.Append("PING", req2.key, req2.value);
    wal1.Append("GET", req3.key, req3.value);
    wal1.Append("SET", req4.key, req4.value);
    wal1.Append("SET", req5.key, req5.value);
    wal1.Append("DEL", req6.key, req6.value);
    wal1.Append("GET", req7.key, req7.value);
    wal1.Append("SET", req8.key, req8.value);
    wal1.Append("DEL", req9.key, req9.value);
    wal1.Append("SET", "test", "test");

    std::ifstream file(FILENAME);
    std::string fLine;
    constexpr size_t TIMESTAMP_SKIP = 22;
    constexpr int NUM_REQS = 10;
    std::array<std::string, NUM_REQS> WALreqs;
    std::size_t i = 0;
    while(std::getline(file, fLine)) {
        std::string skippedLine = fLine.substr(TIMESTAMP_SKIP); // ignore the timestamp
        WALreqs.at(i) = skippedLine;
        ++i;
    }

    check("soidfughnuhg should be in the WAL", WALreqs.at(0) == "soidfughnuhg");
    check("PING should be in the WAL", WALreqs.at(1) == "PING");
    check("GET should be in the WAL", WALreqs.at(2) == "GET");
    check("SET should be in the WAL", WALreqs.at(3) == "SET");
    check("SET key should be in the WAL", WALreqs.at(4) == "SET key");
    check("DEL should be in the WAL", WALreqs.at(5) == "DEL");
    check("GET key should be in the WAL", WALreqs.at(6) == "GET key");
    check("SET key value should be in the WAL", WALreqs.at(7) == "SET key value");
    check("DEL key should be in the WAL", WALreqs.at(8) == "DEL key");
    check("SET test test should be in the WAL", WALreqs.at(9) == "SET test test");

    ThreadSafeCache<std::string, std::string> cache(1000); // default capacity for kvserver
    wal1.Replay(cache);
    check("Only thing left in cache should be: test", cache.Get("test").has_value() && cache.Get("test").value() == "test");

    line();

    // results!!!!!!!!!!!!
    line();
    std::cout << "RESULTS: " << std::endl;
    line();
    std::cout << "PASS: " << GREEN << pass << RESET << std::endl;
    std::cout << "FAIL: " << RED << fail << RESET << std::endl;
    line();
    
    return 0;
}