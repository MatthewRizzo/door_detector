#ifndef CONFIRM_HANDSHAKE_H
#define CONFIRM_HANDSHAKE_H
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

// project defines
#include "comm_utils.h"
#include "constants.h"
#include "handshake_base.h"

using std::cout;
using std::endl;
using std::cerr;

class ConfirmHandshake : virtual public HandshakeBase
{
    public:
        ConfirmHandshake();
        virtual ~ConfirmHandshake();
    protected:
        /**
         * @brief Informs server that their packet was received (basically an ACK)
         */
        void respond_to_server() override;

}; // end ConfirmHandshake

#endif