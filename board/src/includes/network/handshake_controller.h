#ifndef HANDSHAKE_CONTROLLER_H
#define HANDSHAKE_CONTROLLER_H
#include <iostream>

// 3rd party includes
#include <json.hpp>
#include <mutex>

// project defined includes
#include "comm_utils.h"
#include "constants.h"
#include "handshake_base.h"
#include "recv_handshake.h"
#include "confirm_handshake.h"

using std::cout;
using std::endl;
using std::cerr;

class HandshakeController : public RecvHandshake, public ConfirmHandshake
{
    public:
        /**
        * @brief Responsible for establishing communication with server.
        *      Waits for a signal on a pre-established port.
        */
        HandshakeController();
        virtual ~HandshakeController();

        void run_setup_receiver() override;

    private:
        std::mutex mtx;

        // use this to keep track if an update trully happened
        ServerInfo prev_server_val;

}; // end of HandshakeController

#endif