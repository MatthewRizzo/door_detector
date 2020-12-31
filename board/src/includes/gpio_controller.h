#ifndef GPIO_CONTROLLER_H
#define GPIO_CONTROLLER_H

#include <atomic>
#include <cstddef>
#include <iostream>
#include <mutex> // mutex and unique_lock
#include <thread>

// 3rd party includes
#include <wiringPi.h>

// project defined includes
#include "constants.h"
#include "udp_sender.h"
#include "handshake_controller.h"

using std::cout;
using std::cerr;
using std::endl;

class GPIOController
{
    public:
        // Default constructor for before the signal handler
        GPIOController(HandshakeController* handshake);
        virtual ~GPIOController();

        /**
         * @brief Thread function to interface with class and constantly check the sensor.
         *        Does not return anything, but when the door is opened, sends a message to the Server.
         */
        void run_door_thread();

        // stops the thread controlling the door sensor
        GPIOController* stop_running_door_thread();

        // API functions for private variables
        /**
         * @brief Utility function to change the destination ip and port to the server on the fly.
         *          Primarily used within the thread function
         */
        GPIOController* set_server_info(ServerInfo info);
        GPIOController* set_verbosity(std::string is_verbose);
        GPIOController* set_door_sensor_pin(int pin_num);

        /**
         * @return The atomic variable - if true the thread is running
         */
        bool is_thread_running() const;
        GPIOController* set_thread_status(bool new_status);

    private:
        /**
         * @brief Sets up the pin configuration
        **/
        GPIOController* configure_pins();

        /**
         * @brief Reads from the door sensor and does some processing on the result
         * @return true if the door opened, false if closed
         */
        bool read_door_sensor() const;

        // Member variables
        bool is_verbose;
        int door_sensor_pin;
        bool has_been_sent; // ensures no duplicate sends for the same opening

        ServerInfo destination;

        // Used to have access to the up to date server info. should be the static class created in main.
        HandshakeController* handshake;

        // Thread variables
        mutable std::mutex mtx;
        std::atomic_bool keep_checking_sensor;
};

#endif