#include "handshake_base.h"

HandshakeBase::HandshakeBase():
    server_config(),
    board_ip(CommUtil::get_public_ip())
{

    // dont actually start the thread until requested
    continue_receiving.store(false);
}

HandshakeBase::~HandshakeBase()
{}

HandshakeBase* HandshakeBase::set_recv_setup_port(int port)
{
    std::unique_lock<std::mutex> lck (mtx);
    HandshakeBase::recv_port = port;
    return this;
}

HandshakeBase* HandshakeBase::set_send_confirm_port(int port)
{
    std::unique_lock<std::mutex> lck (mtx);
    send_confirm_port = port;
    return this;
}

HandshakeBase* HandshakeBase::set_verbosity(std::string is_verbose)
{
    bool verbose = is_verbose == "true" ? true : false;

    std::unique_lock<std::mutex> lck (mtx);
    this->is_verbose = verbose;
    return this;
}

int HandshakeBase::get_server_port() const
{
    return server_config.port;
}
bool HandshakeBase::get_verbosity() const
{
    return is_verbose;
}

std::string HandshakeBase::get_server_ip() const
{
    return server_config.ip;
}

void HandshakeBase::stop_running_receiver()
{
    // Setting this will stop the main loop of the thread
    continue_receiving.store(false);
    if(get_verbosity())
    {
        cout << "Ending Handshake thread" << endl;
    }
    close_recv_sock();
}

HandshakeBase* HandshakeBase::set_continue_receiving(bool new_recv)
{
    continue_receiving.store(new_recv);
    return this;
}

bool HandshakeBase::get_continue_receiving() const
{
    return continue_receiving.load();
}

std::string HandshakeBase::get_board_ip() const
{
    return board_ip;
}

int HandshakeBase::get_recv_port() const
{
    std::unique_lock<std::mutex> lck (mtx);
    return recv_port;
}

int HandshakeBase::get_send_confirm_port() const
{
    std::unique_lock<std::mutex> lck (mtx);
    return send_confirm_port;
}

ServerInfo HandshakeBase::get_server_config() const
{
    std::unique_lock<std::mutex> lck(mtx);
    ServerInfo tmp(server_config.port, server_config.ip);
    lck.unlock();

    return tmp;
}

HandshakeBase* HandshakeBase::set_server_config(const ServerInfo sender)
{
    std::unique_lock<std::mutex> lck (mtx);
    server_config.ip = sender.ip;
    server_config.port = sender.port;
    return this;
}