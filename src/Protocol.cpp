/* Protocol.cpp */
#include "Protocol.h"
#include <sstream>
#include <unordered_map>

// default constructor
Protocol::Request::Request() : 
cmd(Command::ERROR), key(""), value(""), errorMsg("") {}

// overloaded constructor
Protocol::Request::Request(Command c, const std::string& k, const std::string& v, const std::string& e) : 
cmd(c), key(k), value(v), errorMsg(e) {}

// parse
// parses userinput
Protocol::Request Protocol::Parse(const std::string& userInput) {
    static const std::unordered_map<std::string, Command> lookupTable = {
        {"PING", Command::PING},
        {"SET", Command::SET},
        {"GET", Command::GET},
        {"DEL", Command::DEL},
    };

    Protocol::Request req;
    std::istringstream iss(userInput);
    std::string cmd;
    iss >> cmd;

    auto it = lookupTable.find(cmd);
    if(it != lookupTable.end()) {
        req.cmd = it->second;
    } else {
        req.cmd = Command::ERROR;
        req.errorMsg = "ERROR: Unknown command" + cmd;
        return req;
    }
    
    switch(req.cmd) {
        case Command::PING:
            break;

        case Command::SET: {
            if(!(iss >> req.key >> req.value)) {
                req.cmd = Command::ERROR;
                req.errorMsg = "ERROR: SET requires <key> <value>";
            }
            break;
        }

        case Command::GET: {
            if(!(iss >> req.key)) {
                req.cmd = Command::ERROR;
                req.errorMsg = "ERROR: GET requires <key>";
            }
            break;
        }

        case Command::DEL: {
            if(!(iss >> req.key)) {
                req.cmd = Command::ERROR;
                req.errorMsg = "ERROR: DEL requires <key>";
            }
            break;
        }
    }
    return req;
}

// format
// formats a command into a string to send to Connection
std::string Protocol::Format(const Command cmd, const std::optional<std::string>& result, const std::string& errorMsg) {
    switch(cmd) {
        case Command::PING:
            return "PONG";

        case Command::SET:
            return "OK";

        case Command::GET:
            return result.has_value() ? result.value() : "NOT_FOUND";

        case Command::DEL:
            return result.has_value() ? "DELETED" : "NOT_FOUND";

        case Command::ERROR:
            return errorMsg;
    }
    return "ERROR: Unknown command";
}


