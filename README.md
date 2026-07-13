### KVServer
This is a multithreaded key-value server written in C++ from scratch. Accepts concurrent TCP connections over a simple text protocol:
- **GET**
- **SET**
- **DEL**
- **PING**
The requests are served using a [thread pool](https://github.com/deathybroxd/thread-pool) where each connection is assigned a worker thread. The cache layer for this server is a 
[LRU Cache](https://github.com/deathybroxd/LRU-cache) that I also wrote myself that uses a doubly linked list + hash map for O(1) access.
This cache is wrapped in a mutex-protected thread-safe layer to adapt it for multithreading. This project also comes complete with 
a benchmark client that tests connections and latency under load. All commands are written to a WAL that stores every line of text given to the
server and also allows it to rebuild its cache from the WAL on startup.

- note this is written for linux only, as linux headers are used in this project for socket handling
