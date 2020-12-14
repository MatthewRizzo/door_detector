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

    const std::string LED               {"led"};
    const std::string MULTICAST_PORT    {"b_port"};
}; // end of parse namespace for CLI

namespace Comm
{
    const int DEFAULT_SERVER_PORT         = 55555;
    const int DEFAULT_MULTICAST_PORT      = 55054;
    const std::string MULTICAST_GROUP_IP  = "224.1.1.1";
    const int DATA_BUFFER_SIZE            = 512;
}

#endif