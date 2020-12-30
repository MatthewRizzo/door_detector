#ifndef HANDSHAKE_BASE_H
#define HANDSHAKE_BASE_H
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

class HandshakeBase
{
    public:
            /**
        * @brief Keeps track of base varaibles needed for
        *       establishing communication with server.
        */
        HandshakeBase();
        virtual ~HandshakeBase();

        // API Functions
        /**
         * @pre is_verbose is a string of the form "true" or "false" (from CLI flag)
         */
        HandshakeBase* set_verbosity(std::string is_verbose);
        HandshakeBase* set_recv_setup_port(int port);
        HandshakeBase* set_send_confirm_port(int port);

        bool get_verbosity() const;

        std::string get_server_ip() const;
        int get_server_port() const;
        std::string get_board_ip() const;

        // thread related API's
        HandshakeBase* set_continue_receiving(bool new_recv);
        bool get_continue_receiving() const;

        /**
         * @brief Thread function!!! Wraps other functions in a loop with timeout / sigint killability.
         *  Allows for exit handler to kill thread easily by setting the atomic to false.
         * @note This will keep running, even after initial establishment of comms.
         * Handles the case where server dies and need to restart program
         */
        virtual void run_setup_receiver() = 0;

        /**
         * @brief Sets the atomic such that the threaded function will stop iterating after current loop
         */
        void stop_running_receiver();

        // Functions to be used by RecvHandshake
        // Returns the port to recv on
        int get_recv_port() const;

        // Functions to be used by ConfirmHandshake
        // Returns the port to send confirmation to
        int get_send_confirm_port() const;

        // Functions used by all derived classes
        HandshakeBase* set_server_config(const ServerInfo sender);
        ServerInfo get_server_config() const;

    protected:
        // Functions for recv class
        virtual void close_recv_sock() = 0;
        virtual bool wait_for_setup_msg() = 0;
        virtual bool set_recv_opt(int recv_fd) const = 0;
        virtual std::pair<ServerInfo, bool> rcv_msg(int recv_fd) = 0;
        virtual void set_server_ip_port(const ServerInfo sender) = 0;
        virtual void parse_msg(std::string buf, ServerInfo& server_info) = 0;
        virtual std::string fix_decoded_msg(std::string buf) = 0;

        // Functions for ConfirmHandshake class
        virtual void respond_to_server() = 0;
    private:
        // --------- Member Variables -------//
        bool is_verbose; // used to conntrol the level of print statements made

        // Variables for responding to server
        ServerInfo server_config;

        // Variables for receiving from server
        int recv_port;

        int send_confirm_port;

        // device variables
        std::string board_ip;

        // thread variables
        mutable std::mutex mtx;
        std::atomic_bool continue_receiving;

}; // end of HandshakeBase Declaration

#endif