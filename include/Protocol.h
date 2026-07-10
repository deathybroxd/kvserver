/* Protocol.h
request parser and response formatter
- Connection.h handles the dispatch logic
*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <optional>

namespace Protocol {
    enum class Command { PING, SET, GET, DEL, ERROR };

    struct Request {
        Command cmd;
        std::string key, value, errorMsg;
        
        // default constructor
        Request();

        // overloaded constructor
        Request(Command c, const std::string& k, const std::string& v, const std::string& e);
    };

    // parse
    // parses input taken in from the user
    Request Parse(const std::string& userInput);
    
    // format
    // formats commands into a usable string
    std::string Format(const Command cmd, const std::optional<std::string>& result = std::nullopt, const std::string& errorMsg = "");
}
#endif