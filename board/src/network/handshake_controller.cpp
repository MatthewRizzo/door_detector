#include "handshake_controller.h"

HandshakeController::HandshakeController():
    // Instantiate all classes - public virtual removes their automatic creation
    HandshakeBase(), // do base first
    RecvHandshake(),
    ConfirmHandshake(),
    handshake_cooldown(COMM::HANDSHAKE_COOLDOWN)
{

}

HandshakeController::~HandshakeController()
{
}

void HandshakeController::run_setup_receiver()
{
    std::string msg = "Waiting for server to send a msg on port " + std::to_string(get_recv_port());
    msg += " and ip " + get_board_ip();
    thread_print(msg, true);

    set_continue_receiving(true);
    while(get_continue_receiving() == true)
    {
        // don't start another handshake process until cooldown complete
        complete_cooldown();

        // save the last values, only print the update if they are different
        prev_server_val.ip = get_server_ip();
        prev_server_val.port = get_server_port();


        if(wait_for_setup_msg() == true)
        {
            // respond to server once the setup is complete on this end
            respond_to_server();

            // check if the values have changed
            if(get_server_ip().compare(prev_server_val.ip) != 0
                || get_server_port() != prev_server_val.port)
            {
                std::string msg = "UPDATED SERVER: ip = " + get_server_ip() + ", port = " + std::to_string(get_server_port());
                thread_print(msg, true);
            }

            if(get_verbosity())
            {
                thread_print("Completed another handshake with Server", true);
                thread_print("---------------------------------",true);
                thread_print("",true); // print empty line
            }
        }
    }
}


HandshakeController* HandshakeController::set_handshake_cooldown(int64_t cooldown)
{
    std::unique_lock<std::mutex> lck(mtx);
    handshake_cooldown = cooldown;
    return this;
}

HandshakeController* HandshakeController::complete_cooldown()
{
    // sleep for the duration of the cooldown - stops the thread
    std::this_thread::sleep_for(std::chrono::milliseconds(handshake_cooldown));
    return this;
}
