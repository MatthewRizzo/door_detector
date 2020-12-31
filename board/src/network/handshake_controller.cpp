#include "handshake_controller.h"

HandshakeController::HandshakeController():
    // Instantiate all classes - public virtual removes their automatic creation
    HandshakeBase(), // do base first
    RecvHandshake(),
    ConfirmHandshake()
{

}

HandshakeController::~HandshakeController()
{
}

void HandshakeController::run_setup_receiver()
{
    std::unique_lock<std::mutex> lck(mtx);
    cout << "Waiting for server to send a msg on port " << get_recv_port();
    cout << " and ip " << get_board_ip() << endl;
    lck.unlock();
    set_continue_receiving(true);
    while(get_continue_receiving() == true)
    {
        if(wait_for_setup_msg() == true)
        {
            // respond to server once the setup is complete on this end
            respond_to_server();
        }
        cout << "UPDATED SERVER: ip = " << get_server_ip() << ", port = " << get_server_port() << endl;
    }
}
