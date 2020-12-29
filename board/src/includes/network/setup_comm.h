#ifndef SETUP_COMM_H
#define SETUP_COMM_H
#include <arpa/inet.h> // used by inet_addr
#include <assert.h>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <string.h> // used by memset
#include <mutex> // mutex and unique_lock
#include <thread>
#include <netinet/in.h> // used by sockaddr_in
#include <unistd.h> // used by open/close

// 3rd party includes
#include <json.hpp>

// project defines
#include "comm_utils.h"
#include "constants.h"
#include "json_utils.h"

using std::cout;
using std::endl;
using std::cerr;

using json = nlohmann::json;

struct ServerInfo
{
    std::string ip;
    int port;
};

class SetupComm
{
    public:
        /**
        * @brief Responsible for establishing communication with server.
        *      Waits for a signal on a pre-established port.
        */
        SetupComm();
        virtual ~SetupComm();

        // API Functions
        /**
         * @pre is_verbose is a string of the form "true" or "false" (from CLI flag)
         */
        SetupComm* set_verbosity(std::string is_verbose);
        SetupComm* set_recv_setup_port(int port);
        SetupComm* set_send_confirm_port(int port);

        /**
         * @brief Thread function!!! Wraps other functions in a loop with timeout / sigint killability.
         *  Allows for exit handler to kill thread easily by setting the atomic to false.
         * @note This will keep running, even after initial establishment of comms.
         * Handles the case where server dies and need to restart program
         */
        void run_setup_receiver();

        /**
         * @brief Sets the atomic such that the threaded function will stop iterating after current loop
         */
        void stop_running_receiver();


        std::string get_server_ip();
        int get_server_port();

    private:
        // helper functions

        // These few lines are repeated a lot, so make them into a function
        void close_recv_sock();

        bool set_recv_opt(int recv_fd) const;

        /**
         * @brief Given a pre-structured buf from the server, deocde and
         *      format it into a valid information that can be used by struct sockaddr_in.
         * @return None, but &server_info to contain the relevant values given by the msg
         */
        void parse_msg(std::string buf, ServerInfo& server_info);

        /**
         * @brief Handles cases where received json's have garbage characters making it impossible to parse
         * @param buf The string whose characters are being removed
         */
        std::string fix_decoded_msg(std::string buf);

        /**
         * @brief Abstracts out the receiving of the data and the creation of the addr struct.
         * This function is blocking (but lasts as long as timeout)
         */
        std::pair<ServerInfo, bool> rcv_msg(int recv_fd);

        /**
         * @brief Waits for msg from server, and decodes it.
         * Retrieving/saves the url and port to respond on.
         * @return True on successful receipt of msg.
         */
        bool wait_for_setup_msg();

        /**
         * @brief Informs server that their packet was received (basically an ACK)
         */
        void respond_to_server();

        // --------- Functions to manage member variables -------//
        // Sets the server ip and port after recieving multicast
        void set_server_ip_port(const ServerInfo sender);


        // --------- Member Variables -------//
        bool is_verbose; // used to conntrol the level of print statements made

        // Variables for responding to server
        ServerInfo server_config;

        // fd to use when sending to server
        int server_sock_fd;
        int send_confirm_port;

        // Variables for receiving from server
        int recv_port;
        int recv_sock_fd;

        // device variables
        std::string board_ip;

        // thread variables
        std::mutex mtx;
        std::atomic_bool continue_receiving;

}; // end of SetupComm Declaration

#endif