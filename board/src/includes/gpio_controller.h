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
        GPIOController* set_door_closing_duration(double duration);

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

        /**
         * @brief Only makes changes onn first iteration of while loop after handshake is completed.
         *      \n Sets door_start_status to open or closed depending on its current status.
         *      \n Only changes door_start_status if it started open but closed afterwards.
         */
        GPIOController* manage_door_start_status(bool is_door_open);

        /**
         * @brief Helper to wrap the sending of the msg when applicable
         */
        void manage_sending_msg() const;

        /**
         * @brief If handshake has been completed at least once, destination's values will not be garbage,
         *       and the sensor can be checked.
         */
        bool is_handshake_completed() const;

        /**
         * @brief Used to ensure that the closing of the door does not accidentally trigger a door open message.
         *      In effect, creates a debounce / cooldown on the sensor between state transitions.
         * @param door_opened true if the most recent sensor had the door as opened
         * @return True if the door is closing and the sensor state should be ignored. false otherwise.
         */
        bool ignore_sensor_state(bool door_opened);

        // Member variables
        bool is_verbose;
        int door_sensor_pin;
        mutable bool has_been_sent; // ensures no duplicate sends for the same opening

        // Used to control flow from open -> closed
        GPIO::door_status_codes prev_door_pos;
        std::chrono::_V2::system_clock::time_point door_closing_start_time; // used as relative time for ignoring period
        double closing_duration;

        // If door starts open, don't send a msg until it opens again after being closed
        GPIO::door_start_status door_start_status;

        ServerInfo destination;

        // Used to have access to the up to date server info. should be the static class created in main.
        HandshakeController* handshake;

        // Thread variables
        mutable std::mutex mtx;
        std::atomic_bool keep_checking_sensor;
};

#endif