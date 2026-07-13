/* Connection.cpp */
#include "Connection.h"
#include <sys/socket.h> // for recv() - to recieve msgs from socket
#include <unistd.h> // for close()

// constructor
Connection::Connection(int socketFd, ThreadSafeCache<std::string, std::string>& cache, WAL& wal) : 
m_socketFd(socketFd), m_cache(cache), m_wal(wal) {}

// connection handle
void Connection::Handle() {
    char buf[1024]; // char array to read bytes into
    std::string buffer; 
    ssize_t bytesRead;

    // bytesRead = 0, client disconnect
    // bytesRead = -1, error
    while((bytesRead = recv(m_socketFd, buf, sizeof(buf), 0)) > 0) {
        buffer.append(buf, bytesRead);
        
        // extract line from buffer
        // while(buffer.contains('\n')) in c++ 23
        std::string line;
        while(buffer.find('\n') != std::string::npos) {
            size_t pos = buffer.find('\n');
            line = buffer.substr(0, pos);
            buffer.erase(0, pos + 1);

            // process line
            Protocol::Request req = Protocol::Parse(line);
            std::optional<std::string> result;
            switch(req.cmd) {
                case Protocol::Command::PING:
                    result = std::nullopt;
                    break;

                case Protocol::Command::SET:
                    m_cache.Set(req.key, req.value);
                    m_wal.Append("SET", req.key, req.value); // add this new kv pair to the WAL
                    result = std::nullopt;
                    break;

                case Protocol::Command::GET:
                    result = m_cache.Get(req.key);
                    break;

                case Protocol::Command::DEL:
                    result = m_cache.Remove(req.key);
                    m_wal.Append("DEL", req.key, req.value); // add this new kv pair to the WAL
                    break;
            
                default:
                    result = std::nullopt;
                    break;
            }
            std::string response = Protocol::Format(req.cmd, result, req.errorMsg) + '\n';
            send(m_socketFd, response.c_str(), response.size(), 0);
        }
    }
    close(m_socketFd);
}

