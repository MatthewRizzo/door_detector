// Standard Includes
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// 3rd Party Includes

// Project Includes
#include "cli_controller.h"
#include "constants.h"
#include "setup_comm.h"

using std::cerr;
using std::endl;
using std::cout;
using tvec = std::vector<std::thread>;


int main(int argc, char* argv[])
{
    //TODO: Startup client thread
    static SetupComm setup;

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
        setup.stop_running_receiver();


    });

    //-------------- Initialize and Start --------------//
    tvec threads;

    // Setup Comms initialization and start
    setup.set_verbosity(parse_res["v_setup"])
        ->set_recv_setup_port(std::stoi(parse_res["sr_port"]))
        ->set_send_confirm_port(std::stoi(parse_res["ss_port"]));

    threads.push_back(std::thread(&SetupComm::run_setup_receiver, std::ref(setup)));

    // close all threads
    for(auto &thread : threads)
    {
        if(thread.joinable())
        {
            thread.join();
        }
    }


    // setup.start_setup_thread();

    return EXIT_SUCCESS;
}