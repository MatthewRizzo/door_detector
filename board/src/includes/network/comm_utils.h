#ifndef COMM_UTILS_H
#define COMM_UTILS_H

#include <arpa/inet.h> // used by inet_addr
#include <assert.h>
#include <iostream>
#include <string.h> // used by memset
#include <netinet/in.h> // used by sockaddr_in
#include <unistd.h> // used by open/close

using std::endl;
using std::cout;

class CommUtil
{
public:
    /**
     * @brief Get the public ip of the device
     * @credit https://stackoverflow.com/a/3120382/13933174 (with some modifications)
     * @return The ip address of the board as seen by other devices
     */
    static std::string get_public_ip();
};

#endif