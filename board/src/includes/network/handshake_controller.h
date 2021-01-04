#ifndef HANDSHAKE_CONTROLLER_H
#define HANDSHAKE_CONTROLLER_H

#include <iostream>
#include <chrono> // std::chrono::seconds

// 3rd party includes
#include <json.hpp>
#include <mutex>
#include <thread>

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

        // Getters / setters
        /**
         * @brief Used to set the cooldown between handshakes after the creation of the object (because done statically)
         */
        HandshakeController* set_handshake_cooldown(int64_t cooldown);

    private:

        // use this to keep track if an update trully happened
        ServerInfo prev_server_val;

        // used to control how often handshake is done
        int64_t handshake_cooldown;
        HandshakeController* complete_cooldown();

        // Thread variables
        std::mutex mtx;

}; // end of HandshakeController

#endif