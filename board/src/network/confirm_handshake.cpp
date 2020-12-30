#include "confirm_handshake.h"

ConfirmHandshake::ConfirmHandshake(){}
ConfirmHandshake::~ConfirmHandshake(){}

void ConfirmHandshake::respond_to_server()
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
    send_dest.sin_port = htons(get_send_confirm_port());
    if(get_verbosity() == true)
    {
        cout << "Acknowledging setup completion by sending ACK to server (computer)";
        cout << " port " << get_send_confirm_port() << " and IP: " << get_server_ip() << endl;
    }
    if(sendto(response_sock_fd, (const char*) send_buf, sizeof(send_buf), 0, (sockaddr *) &send_dest, sizeof(send_dest)) < 0)
    {
        cout << "ERROR: sending response packet confirming setup" << endl;
    }
    close(response_sock_fd);
    if(get_verbosity() == true)
    {
        cout << "---------------------------------" << endl << endl;
    }
}