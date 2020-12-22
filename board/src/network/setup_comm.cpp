#include "setup_comm.h"

SetupComm::SetupComm():
    server_sock_fd(-1),
    send_confirm_port(Comm::DEFAULT_SETUP_SEND_PORT),
    recv_port(Comm::DEFAULT_SETUP_RECV_PORT),
    board_ip(CommUtil::get_public_ip())
{
    server_config.ip = "";
    server_config.port = Comm::DEFAULT_SERVER_PORT;

    // default to allow  receipt of msg from server
    continue_receiving.store(true);
}

SetupComm::~SetupComm()
{
    close_recv_sock();
}

void SetupComm::run_setup_receiver()
{
    // sets to true for edge cases on startup of thread
    continue_receiving.store(true);
    cout << "Waiting for server to send a msg on port " << recv_port;
    cout << " and ip " << board_ip << endl;
    while(continue_receiving.load() == true)
    {
        if(wait_for_setup_msg() == true)
        {
            // respond tp server once the setup is complete on this end
            respond_to_server();
        }
    }
}

bool SetupComm::wait_for_setup_msg()
{
    // Setup the socket to wait
    recv_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(recv_sock_fd < 0)
    {
        cout << "ERROR opening recv socket" << endl;
        return false;
    }

    // setup options - for multicast mode allow reuse
    if(set_recv_opt(recv_sock_fd) == false)
    {
        close_recv_sock();
        return false;
    }

    // Create a struct to store the source Ip of the broadcast message
    const std::pair<struct ServerInfo, bool> sender_info {rcv_msg(recv_sock_fd)};
    if(sender_info.second == true)
    {
        set_server_ip_port(sender_info.first);
    }

    // Close the socket before starting the process again
    close_recv_sock();


    return std::get<1>(sender_info);
}

void SetupComm::set_recv_setup_port(int port)
{
    std::unique_lock<std::mutex> lck (mtx);
    recv_port = port;
}

void SetupComm::set_send_confirm_port(int port)
{
    std::unique_lock<std::mutex> lck (mtx);
    send_confirm_port = port;
}

void SetupComm::set_server_ip_port(const ServerInfo sender)
{
    std::unique_lock<std::mutex> lck (mtx);
    server_config.ip = sender.ip;
    server_config.port = sender.port;
}

int SetupComm::get_server_port()
{
    return server_config.port;
}

std::string SetupComm::get_server_ip()
{
    return server_config.ip;
}

void SetupComm::stop_running_receiver()
{
    // Setting this will stop the main loop of the thread
    continue_receiving.store(false);
    close_recv_sock();
}

// helper functions
bool SetupComm::set_recv_opt(int recv_sock_fd) const
{
    // Add option to re-use socket port
    int option(1);
    if(setsockopt(recv_sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0)
    {
        cout << "ERROR: setting SO_REUSEADDR failed" << endl;
        close(recv_sock_fd);
        return false;
    }
    if(setsockopt(recv_sock_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&option, sizeof(option)) < 0)
    {
        cout << "ERROR: setting SO_REUSEPORT failed" << endl;
        close(recv_sock_fd);
        return false;
    }

    // Add the option for timeout on the socket (allows for exit handler to kill socket)
    struct timeval tv;
    tv.tv_sec = Comm::TIMEOUT_SEC;
    tv.tv_usec = Comm::TIMEOUT_SEC;
    if (setsockopt(recv_sock_fd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0)
    {
        std::cerr << "ERROR: Adding timeout to Multicast socket" << endl;
    }

    // bind to the correct port and ip address (any)
    struct sockaddr_in send_socket;
    memset((char *) &send_socket, 0, sizeof(send_socket));
    send_socket.sin_family = AF_INET;
    send_socket.sin_port = htons(recv_port);
    send_socket.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(recv_sock_fd, (struct sockaddr*) &send_socket, sizeof(send_socket)) < 0)
    {
        cout << "ERROR: Binding the recv socket failed. " << endl;
        close(recv_sock_fd);
        return false;
    }

    return true;
}

std::pair<struct ServerInfo, bool> SetupComm::rcv_msg(int recv_sock_fd)
{
    ServerInfo sender_socket_info;
    memset((char *) &sender_socket_info, 0, sizeof(sender_socket_info));

    // Read from the socket
    char data_buffer[Comm::DATA_BUFFER_SIZE];
    ssize_t rcv_size = recv(recv_sock_fd, data_buffer, sizeof(data_buffer), 0);
    if(rcv_size < 0)
    {
        // most likely this occurs due to timeout
        close_recv_sock();
        return std::make_pair(sender_socket_info, false);
    }
    cout << "Received Setup Message: " << std::string(data_buffer) << endl;
    parse_msg(std::string(data_buffer), sender_socket_info);
    return std::make_pair(sender_socket_info, true);
}

void SetupComm::parse_msg(std::string buf, ServerInfo &server_info)
{
    // remove garbage characters - artifact of serializing json and reading it into c++
    std::string buf_no_extra = fix_decoded_msg(buf);

    const json& data = json::parse(buf_no_extra);
    bool has_host = data.contains(Comm::HOST_KEY);
    bool has_ip = data.contains(Comm::IP_KEY);
    bool has_port = data.contains(Comm::PORT_KEY);

    // if both ip and hostname are given, default to ip
    if(has_ip) {
        // This temporary variable is redundant, but if removed causes strange seg faults
        //TODO: fix this bug. what is cause??
        std::string ip = data[Comm::IP_KEY].get<std::string>();
        server_info.ip = ip;
    }
    else if(has_host)
    {
        std::string ip = data[Comm::HOST_KEY].get<std::string>();
        server_info.ip = ip;
    }

    if(has_port)
    {
        server_info.port =  data[Comm::PORT_KEY].get<int>();
    }
    // cout << "port = " << server_info.port << ", ip = " << server_info.ip << endl;
}

void SetupComm::close_recv_sock()
{
    close(recv_sock_fd);
    recv_sock_fd = -1;
}

void SetupComm::respond_to_server()
{
    // create the socket
    int response_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(response_sock_fd < 0)
    {
        cout << "ERROR: failed to create reponse socket confirming setup" << endl;
        return;
    }

    // bind to the send correct port and ip address (any)
    struct sockaddr_in send_socket;
    memset((char *) &send_socket, 0, sizeof(send_socket));
    send_socket.sin_family = AF_INET;
    send_socket.sin_port = htons(Comm::DEFAULT_SETUP_SEND_BIND_PORT);
    send_socket.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(response_sock_fd, (struct sockaddr*) &send_socket, sizeof(send_socket)) < 0)
    {
        cout << "ERROR: Binding the send socket failed. " << endl;
        close(response_sock_fd);
        return;
    }

    // send the ack
    char send_buf[1024] = "ACK";
    struct sockaddr_in send_dest;
    send_dest.sin_addr.s_addr = inet_addr(get_server_ip().c_str());
    send_dest.sin_family = AF_INET;
    send_dest.sin_port = htons(send_confirm_port);
    cout << "Acknowledging setup completion by sending ACK to server (computer)";
    cout << " port " << send_confirm_port << " and IP: " << get_server_ip() << endl;
    if(sendto(response_sock_fd, (const char*) send_buf, sizeof(send_buf), 0, (sockaddr *) &send_dest, sizeof(send_dest)) < 0)
    {
        cout << "ERROR: sending response packet confirming setup" << endl;
    }
    close(response_sock_fd);
    cout << "---------------------------------" << endl << endl;
}

std::string SetupComm::fix_decoded_msg(std::string buf)
{
    // keep removing the last character until an exception is not thrown
    std::string cur_str = buf;
    bool fixed = false;

    while (fixed == false)
    {
        // each iteration try to parse the string
        // if it fails, move back 1 char
        try{
            const json& data = json::parse(cur_str);
            fixed = true;
        }catch(std::exception& error)
        {
            cur_str.pop_back();
        }
    }
    return cur_str;
}