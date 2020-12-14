// Standard Includes
#include <csignal>
#include <iostream>
#include <string>
#include <vector>

// 3rd Party Includes

// Project Includes
#include "cli_controller.h"
#include "client.h"
#include "constants.h"

using std::cerr;
using std::endl;
using std::cout;

int main(int argc, char* argv[])
{
    //TODO: Startup client thread
    Client client;

    //TODO: create GPIO object
    // led list will come from here
    std::vector<std::string> led_list {"RED", "BLUE"};

    //-------------- Parse for CLI Flags --------------//
    CLI_Controller::CLI_parser parser(
        argc,
        argv,
        led_list
    );


    CLI::ParseMetaData::parse_results parse_res;
    try
    {
        parse_res = parser.parse_flags();
    }catch (std::runtime_error& err){
        return EXIT_FAILURE;
    }

    //-------------- Create Ctrl+C Handler --------------//
    std::signal(SIGINT, [](int signum) {
        cout << "Caught ctrl+c: " << signum << endl;
        // Include any call backs to join/kill threads
    });

    //-------------- Initialize and Start --------------//
    cout << "parse_res = " << parse_res["led"] << endl;

    // Client initialization and start
    client.set_multicast_port(std::stoi(parse_res["b_port"]));
    client.wait_for_multicast();

    return EXIT_SUCCESS;
}