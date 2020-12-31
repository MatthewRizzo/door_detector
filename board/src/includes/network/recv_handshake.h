#ifndef SEND_HANDSHAKE_H
#define SEND_HANDSHAKE_H
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

// project defined includes
#include "comm_utils.h"
#include "constants.h"
#include "json_utils.h"
#include "handshake_base.h"

using std::cout;
using std::endl;
using std::cerr;

using json = nlohmann::json;

class RecvHandshake : virtual public HandshakeBase
{
    public:
        RecvHandshake();
        virtual ~RecvHandshake();

    protected:
        void close_recv_sock() override;

        /**
         * @brief Waits for msg from server, and decodes it.
         * Retrieving/saves the url and port to respond on.
         * @return True on successful receipt of msg.
         */
        bool wait_for_setup_msg() override;

        bool set_recv_opt(int recv_fd) const override;

        /**
         * @brief Abstracts out the receiving of the data and the creation of the addr struct.
         * This function is blocking (but lasts as long as timeout)
         */
        std::pair<ServerInfo, bool> rcv_msg(int recv_fd) override;

    private:
        // Sets the server ip and port after recieving handshake
        void set_server_ip_port(const ServerInfo sender) override;

        /**
         * @brief Given a pre-structured buf from the server, deocde and
         *      format it into a valid information that can be used by struct sockaddr_in.
         * @return None, but &server_info to contain the relevant values given by the msg
         */
        void parse_msg(std::string buf, ServerInfo& server_info) override;

        /**
         * @brief Handles cases where received json's have garbage characters making it impossible to parse
         * @param buf The string whose characters are being removed
         */
        std::string fix_decoded_msg(std::string buf) override;

        // Variables for receiving from server
        int recv_sock_fd;

        //thread variables
        std::mutex mtx;
}; // end of RecvHandshake class

#endif