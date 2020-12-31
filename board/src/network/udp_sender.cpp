#include "udp_sender.h"

UDPSender::UDPSender(int dest_port, std::string dest_ip, bool is_verbose):
    dest(dest_port, dest_ip),
    sock_fd(-1),
    is_verbose(is_verbose)
{}

UDPSender::~UDPSender()
{
    close_sock();
}

bool UDPSender::send_msg(std::string msg)
{
    if(create_sock() == false)
    {
        return false;
    }

    if(opt_and_bind() == false)
    {
        return false;
    }

    return perform_send(msg);

}

bool UDPSender::perform_send(std::string msg)
{
    const char* c_msg = msg.c_str();

    struct sockaddr_in send_dest;
    send_dest.sin_addr.s_addr = inet_addr(dest.ip.c_str());
    send_dest.sin_family = AF_INET;
    send_dest.sin_port = htons(dest.port);

    if(is_verbose == true)
    {
        cout << "Sending msg to ip" << dest.ip << " on port " << dest.port << endl;
    }
    if(sendto(sock_fd, c_msg, sizeof(c_msg), 0, (sockaddr *) &send_dest, sizeof(send_dest)) < 0)
    {
        cout << "ERROR: sending msg to ip" << dest.ip << " on port " << dest.port << endl;
        return false;
    }
    close_sock();
    return true;

}

bool UDPSender::create_sock()
{
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)
    {
        cout << "ERROR: failed to create udp_sender socket" << endl;
        return false;
    }

    return true;
}

void UDPSender::close_sock()
{
    if(sock_fd > -1)
    {
        close(sock_fd);
        sock_fd = -1;
    }
}

bool UDPSender::opt_and_bind()
{
    struct sockaddr_in send_socket;
    memset((char *) &send_socket, 0, sizeof(send_socket));
    send_socket.sin_family = AF_INET;
    send_socket.sin_port = htons(dest.port);
    send_socket.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock_fd, (struct sockaddr*) &send_socket, sizeof(send_socket)) < 0)
    {
        cout << "ERROR: Binding the send socket failed. " << endl;
        close(sock_fd);
        return false;
    }
    return true;
}