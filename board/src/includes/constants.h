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

    const std::string LED         {"led"};
}; // end of parse namespace for CLI

#endif