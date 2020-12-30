#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include <arpa/inet.h> // used by inet_addr
#include <assert.h>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <string.h> // used by memset
#include <mutex> // mutex and unique_lock
#include <thread>
#include <tuple>
#include <netinet/in.h> // used by sockaddr_in
#include <unistd.h> // used by open/close

using std::cout;
using std::endl;
using std::cerr;

using rcode = std::pair<bool, std::string>;

struct DestInfo
{
    std::string ip;
    int port;
    DestInfo(int dest_port, std::string dest_ip):
        port(dest_port), ip(dest_ip)
        {};
};

class UDPSender
{
    public:
        /**
         * @brief Used to abstract out the sending of packets over udp
         */
        UDPSender(int dest_port, std::string dest_ip, bool is_verbose);
        virtual ~UDPSender();

        bool send_msg(std::string msg);
    private:
        //helper functions
        bool create_sock();
        bool opt_and_bind();
        void close_sock();

        // Function to actually send the msg
        bool perform_send(std::string msg);

        // class members
        DestInfo dest;
        int sock_fd;
        bool is_verbose;

}; // End of UDPSender

#endif