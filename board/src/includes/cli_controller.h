#ifndef CLI_PROC_H
#define CLI_PROC_H

// Standard Includes
#include <iostream>
#include <string>
#include <vector>


// 3rd Party Includes
#include <CLI.hpp>

// Project Includes
#include "constants.h"

using std::cerr;
using std::endl;
using std::cout;

namespace CLI_Controller {

class CLI_parser : public CLI::App
{
    public:
        /**
         * @brief Create a CLI_Controller that can processes arguments for the client and GPIO process
         * @param _argc The number of args
         * @param _argv An array of string-literals representing each argument
         * @param _led_list List of LED's to use (default to all unused)
         * @param name The name of the parser
         */
        explicit CLI_parser(
            int _argc,
            char* _argv[],
            const std::vector<std::string> _led_list,
            const std::string name="Door Detection Parser"
        );

        /**
         * @brief Parses the CLI Flags. Results are returned in a map
         * @return The results in a map
         * @note A throw will happen if error encountered during parse
         */
        const CLI::ParseMetaData::parse_results& parse_flags() noexcept(false);
    private:
        // - Private Variables - //
        // Store the # of args
        int argc;

        // Store the arguments
        char** argv;

        // list of led's to turn on
        const std::vector<std::string>& led_list;

        CLI::ParseMetaData::parse_results cli_results;

        // - helper Functions - //
        /**
         * @brief Adds the flags/arguments checking for to the parser
         * @return ResultCodes
         */
        void setup_flags();


}; // end of class declaration
}; // end of namespace

#endif