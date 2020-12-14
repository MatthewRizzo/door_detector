#ifndef CLIENT_H
#define CLIENT_H

// Standard Includes
#include <arpa/inet.h> // used by inet_addr
#include <assert.h>
#include <iostream>
#include <string.h> // used by memset
#include <netinet/in.h> // used by sockaddr_in
#include <unistd.h> // used by open/close


class Client
{
    public:
        Client();

        // API functions
        /**
         * @brief Informs the server that the door has opened. Uses information from multicast.
         * @return True on successful alert.
         */
        bool alert_server();

        void set_multicast_port(int port);

        /**
         * @brief Waits for multicast from server, and decodes the msg.
         * Retrieving/saves the url and port to respond on.
         * @return True on successful receipt of multicast.
         */
        bool wait_for_multicast();
    private:
        /**
         * @brief Get the public ip of the device
         * @credit https://stackoverflow.com/a/3120382/13933174 (with some modifications)
         * @return The ip address of the board as seen by other devices
         */
        std::string get_public_ip() const;

        /**
         * @return The ip address of the sender
         */
        std::string get_sender_ip(struct sockaddr_in sender) const;

        /**
         * @return The port the sender used
         */
        int get_sender_port(struct sockaddr_in sender) const;

        // Variables for responding to server
        std::string server_ip;
        int server_port;

        // fd to use when sending to server
        int server_sock_fd;

        // Variables for multicast receipt
        int multicast_port;
        int multicast_sock_fd;

        // device variables
        std::string device_ip;



}; // End of Client class

#endif