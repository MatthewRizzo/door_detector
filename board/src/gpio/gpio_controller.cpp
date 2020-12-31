#include "gpio_controller.h"

// Default constructor when making the static object before signal handler
GPIOController::GPIOController(HandshakeController* handshake):
    is_verbose(GPIO::DEFAULT_VERBOSITY),
    door_sensor_pin(GPIO::DEFAULT_DOOR_SENSOR_PIN),
    has_been_sent(false),
    door_start_status(GPIO::door_start_status::first_iteration),
    destination(), // use default constructor to get garbage values
    handshake(handshake)
{
    // The thread should default to not running
    set_thread_status(false);
    configure_pins();
}

GPIOController::~GPIOController()
{
}

GPIOController* GPIOController::stop_running_door_thread()
{
    set_thread_status(false);
    if(is_verbose == true)
    {
        cout << "Stopping Door Sensor Thread" << endl;
    }
    return this;
}

void GPIOController::run_door_thread()
{
    set_thread_status(true);
    while(is_thread_running() == true)
    {
        // Keep updating destination using the current values from handshake thread
        set_server_info(handshake->get_server_config());

        // Only run the thread if they are non-default garbage values
        if(!is_handshake_completed()) {continue;}

        bool door_opened = read_door_sensor();

        // check if door started open (only on first iteration of loop)
        manage_door_start_status(door_opened);

        if(door_opened)
        {
            manage_sending_msg();
        }
        else{
            // Allows msgs to get sent once the door actually opens
            door_start_status = GPIO::door_start_status::closed_at_least_once;

            // reset the conditional once the door is closed
            has_been_sent = false;
        }
    }
}

bool GPIOController::is_thread_running() const
{
    bool status = keep_checking_sensor.load();

    return status;
}

GPIOController* GPIOController::set_thread_status(bool new_status)
{
    keep_checking_sensor.store(new_status);
    return this;
}

GPIOController* GPIOController::set_server_info(ServerInfo info)
{
    // uses implicit copy constructor
    destination = info;
    return this;
}

GPIOController* GPIOController::set_verbosity(std::string is_verbose)
{
    bool verbose = is_verbose == "true" ? true : false;

    std::unique_lock<std::mutex> lck (mtx);
    this->is_verbose = verbose;
    return this;
}

GPIOController* GPIOController::set_door_sensor_pin(int pin_num)
{
    std::unique_lock<std::mutex> lck (mtx);
    door_sensor_pin = pin_num;
    lck.unlock();

    // update pin configuration
    return configure_pins();
}

bool GPIOController::read_door_sensor() const
{
    int status = digitalRead(door_sensor_pin);
    GPIO::door_status_codes converted_status = static_cast<GPIO::door_status_codes>(status);
    return converted_status == GPIO::door_status_codes::open;
}

GPIOController* GPIOController::configure_pins()
{
    // Get wiring pi to do its internal setup
    wiringPiSetup();

    // Set the pin to input
    pinMode(door_sensor_pin, GPIO::direction.at("in"));

    // Make the internal resistor pull up - based on http://wiringpi.com/examples/gertboard-and-wiringpi/buttons/
    pullUpDnControl(door_sensor_pin, GPIO::pull_codes.at("pull_up"));

    return this;
}

GPIOController* GPIOController::manage_door_start_status(bool is_door_open)
{
    if(door_start_status == GPIO::door_start_status::first_iteration)
    {
        door_start_status = is_door_open ?
                    GPIO::door_start_status::started_open :
                    GPIO::door_start_status::closed_at_least_once;
    }
}

GPIOController* GPIOController::manage_sending_msg() const
{
    // don't send if door is open, but it started open
    // only send 1 msg per door opening
    if(has_been_sent == false and door_start_status != GPIO::door_start_status::started_open)
    {
        if(is_verbose)
        {
            std::unique_lock<std::mutex> lck(mtx);
            cout << "Sending notification packet that door has opened" << endl;
            lck.unlock();
        }

        UDPSender sender(destination.port, destination.ip, is_verbose);
        sender.send_msg("Door Opened");
        has_been_sent = true;
    }
}

bool GPIOController::is_handshake_completed() const
{
    bool res = true;
    // fail if port still garabge value
    res &= destination.port != COMM::GARBAGE_SERVER_PORT;

    // When compare == 0, strings are the same (still garbage)
    res &= destination.ip.compare(COMM::GARBAGE_SERVER_IP) != 0;
    return res;
}