#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef>
#include <map>
#include <string>

// 3rd party includes
#include <wiringPi.h>

// project defined includes

enum class ResultCodes
{
    Ok,
    Error,
};

namespace CLI::ParseMetaData
{
    // Makes the future mappings shorter
    using parse_results = std::map<std::string, std::string>;

    const std::string LED                {"led"};

    // related to Comms
    const std::string SETUP_RECV_PORT    {"sr_port"};
    const std::string SETUP_SEND_PORT    {"ss_port"};
    const std::string VERBOSE_SETUP      {"v_setup"};
    const std::string HANDSHAKE_COOLDOWN {"cooldown"};

    // Related to GPIO
    const std::string VERBOSE_GPIO       {"v_gpio"};
    const std::string DOOR_SENSOR_PIN    {"door_sensor_pin"};
    const std::string CLOSING_DURATION   {"close_duration"};

}; // end of parse namespace for CLI

namespace COMM
{
    const bool DEFAULT_VERBOSITY            {false};
    constexpr int64_t HANDSHAKE_COOLDOWN      {2000}; // msec

    constexpr int DATA_BUFFER_SIZE          {1024};

    constexpr int TIMEOUT_SEC               {10}; // Used for heartbeat on sockets
    constexpr int TIMEOUT_MILLISEC          {0};

    // used in setup process //
    constexpr int DEFAULT_SETUP_RECV_PORT      {52535}; // must be same when server sends
    // port being sent to on computer
    constexpr int DEFAULT_SETUP_SEND_PORT      {55554}; // must match CONFIRMATION_WAIT_PORT in python code
    constexpr int DEFAULT_SETUP_SEND_BIND_PORT {55553}; // port being sent FROM during confirm

    // JSON related
    const std::string HOST_KEY              {"host"};
    const std::string IP_KEY                {"ip"};
    const std::string PORT_KEY              {"port"};

    // Related to server config before handshake
    constexpr int GARBAGE_SERVER_PORT       {-1};
    const std::string GARBAGE_SERVER_IP     {"-1"};

}

namespace GPIO
{
    const bool DEFAULT_VERBOSITY            {false};

    // definitions for this namespace come from http://wiringpi.com/reference/core-functions/
    using wiring_codes = std::map<std::string, int>;

    constexpr int DEFAULT_DOOR_SENSOR_PIN    {1};
    const wiring_codes pull_codes
    {
                                        {"no_pull", PUD_OFF},
                                        {"ground_pull", PUD_DOWN},
                                        {"pull_up", PUD_UP} // usually needed for inputs / sensors
    };
    const wiring_codes direction
    {
                                        {"in", INPUT},
                                        {"out", OUTPUT}
    };
    enum class door_status_codes
    {
        // door is closed -> sensors together -> read a 0 = low
        closed    = LOW,
        open      = HIGH,
        unknown   = -1, // Used for when position is unknown
        closing   = 100 // Used when door is in the process of closing
    };

    // Used to control how long sesnor state is ignored during door closing
    constexpr int64_t DEFAULT_CLOSING_PERIOD  {500}; // milliseconds

    enum class door_start_status
    {
        // Where the "first" iteration is relative to handshake having been completed
        first_iteration         = -1,
        started_open            = 0,

        // Once the door has been closed once (either at start or after, notifcations start)
        closed_at_least_once    = 1,
    };
}

#endif