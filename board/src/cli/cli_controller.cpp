// Project Includes
#include "cli_controller.h"

// open the namespace for ease of definition
namespace CLI_Controller {

CLI_parser::CLI_parser(
    int _argc, char* _argv[],
    const std::vector<std::string> _led_list, const std::string name)
//Use parent constructor
:CLI::App(name),
argc{_argc},
argv{_argv},
led_list{_led_list}
{
    // add flags to be able to scan
    setup_flags();
}


// Parse functions
const CLI::ParseMetaData::parse_results&
CLI_parser::parse_flags() noexcept(false)
{
    // perform the parse
    try
    {
        CLI::App::parse(argc, argv);
    }catch (const CLI::ParseError& err){
        cerr << "--------Failed to parse CLI Flags!--------" << endl;
        CLI::App::exit(err); // abstracts handling of error msgs
        throw(err);
    }

    // Return the results
    return cli_results;
}

// Setup for parsing functions
void CLI_parser::setup_flags()
{
    // each option passes the flag value into the cli_results map

    const char delim {','};
    add_option("-l,--leds", cli_results[CLI::ParseMetaData::LED])
        ->description("Which LED(s) do you want to be used? (comma seperated)")
        ->required(false)
        ->default_val("")
        // Way to take in multiple arguments - str vector
        ->expected(0, led_list.size()-1)
        ->allow_extra_args() // allows multiple inputs, even though the type is a single str
        ->delimiter(delim)
        ->check(CLI::IsMember(led_list))
        ->join(delim);

    add_option("-r,--setup_recv_port",
            cli_results[CLI::ParseMetaData::SETUP_RECV_PORT])
        ->description("Which port do you want to use for the Setup wait (recv) socket?")
        ->required(false)
        ->default_val(Comm::DEFAULT_SETUP_RECV_PORT);

    add_option("-s,--setup_send_port",
            cli_results[CLI::ParseMetaData::SETUP_SEND_PORT])
        ->description("Which port do you want to use for the Setup confirm (send) socket?")
        ->required(false)
        ->default_val(Comm::DEFAULT_SETUP_SEND_PORT);
}

}; // end of namespace