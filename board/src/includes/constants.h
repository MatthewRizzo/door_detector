#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <map>

enum class ResultCodes
{
    Ok,
    Error,
};

namespace CLI::ParseMetaData
{
    // Makes the future mappings shorter
    using parse_results = std::map<std::string, std::string>;

    const std::string LED                {"led"};
    const std::string SETUP_RECV_PORT    {"sr_port"};
    const std::string SETUP_SEND_PORT    {"ss_port"};
}; // end of parse namespace for CLI

namespace Comm
{
    constexpr int DEFAULT_SERVER_PORT       {55555}; // used when sending to server
    constexpr int DATA_BUFFER_SIZE          {1024};


    constexpr int TIMEOUT_SEC               {10}; // Used for heartbeat on sockets
    constexpr int TIMEOUT_MILLISEC          {0};

    // used in setup process //
    constexpr int DEFAULT_SETUP_RECV_PORT   {52535}; // must be same when server sends
    // port being sent to on computer
    constexpr int DEFAULT_SETUP_SEND_PORT   {55554}; // must match CONFIRMATION_WAIT_PORT in python code
    constexpr int DEFAULT_SETUP_SEND_BIND_PORT {55553}; // port being sent FROM during confirm

    // JSON related
    const std::string HOST_KEY              {"host"};
    const std::string IP_KEY                {"ip"};
    const std::string PORT_KEY              {"port"};
}

#endif