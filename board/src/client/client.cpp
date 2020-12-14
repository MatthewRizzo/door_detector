// Project Includes
#include "client.h"
#include "constants.h"

using std::cout;
using std::endl;

// Constructor
Client::Client():
    server_ip(""),
    server_port(Comm::DEFAULT_SERVER_PORT),
    server_sock_fd(-1),
    multicast_port(Comm::DEFAULT_MULTICAST_PORT),
    device_ip(get_public_ip())
{
}

void Client::set_multicast_port(int port)
{
    multicast_port = port;
}

bool Client::wait_for_multicast()
{
    //TODO: Make this shorter / break into smaller pieces

    // Setup the socket to wait
    multicast_sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(multicast_sock_fd < 0)
    {
        cout << "ERROR opening multicast socket" << endl;
        return false;
    }

    // setup options - for multicast mode allow reuse
    int option(1);
    if(setsockopt(multicast_sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &option, sizeof(option)) < 0)
    {
        cout << "ERROR setting socket options for multicast. Closing the socket" << endl;
        close(multicast_sock_fd);
        return false;
    }

    // Setup multicast socket addr
    struct sockaddr_in multicast_sock_addr;
    memset((char *) &multicast_sock_addr, 0, sizeof(multicast_sock_addr));
    multicast_sock_addr.sin_family = AF_INET;
    multicast_sock_addr.sin_port = htons(multicast_port); // convert port to network
    // only wait for multicast. give group addr
    multicast_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Try to bind the waiting socket
    if(bind(multicast_sock_fd, (struct sockaddr* ) &multicast_sock_addr, sizeof(multicast_sock_addr)) < 0)
    {
        cout << "ERROR: Binding the multicast socket failed. " << endl;
        close(multicast_sock_fd);
        return false;
    }
    cout << "Successfully bound the multicast socket" << endl;

    // Join the multicast group on the addr given in the python constants file
    /* Note that this IP_ADD_MEMBERSHIP option must be called
        for each local interface over which the multicast
        datagrams are to be received.*/
    struct ip_mreq multicast_group;
    // This is the address of the group - corresponds to server's MULTICAST_GROUP_IP
    multicast_group.imr_multiaddr.s_addr = inet_addr(Comm::MULTICAST_GROUP_IP.c_str());
    multicast_group.imr_interface.s_addr = inet_addr(device_ip.c_str());

    // Set the options for multicast
    if(setsockopt(multicast_sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&multicast_group, sizeof(multicast_group)) < 0)
    {
        std::cerr << "ERROR: Adding multicast membership" << endl;
        close(multicast_sock_fd);
        return false;
    }
    cout << "Successfully added to multicast group" << endl;

    // Create a struct to store the source Ip of the broadcast message
    struct sockaddr_in sender_socket_info;
    memset((char *) &sender_socket_info, 0, sizeof(sender_socket_info));

    // Read from the socket
    cout << "Waiting to receive multicast message" << endl;
    char data_buffer[Comm::DATA_BUFFER_SIZE];
    ssize_t rcv_size = ::recvfrom(multicast_sock_fd, data_buffer, sizeof(data_buffer), 0,
                                (sockaddr *) &sender_socket_info, (socklen_t *) sizeof(sender_socket_info));
    if(rcv_size < 0)
    // if(read(multicast_sock_fd, data_buffer, sizeof(data_buffer)) < 0 )
    {
        std::cerr << "ERROR: Reading Multicast Message" << endl;
        close(multicast_sock_fd);
        return false;
    }
    cout << "Received Multicast Message: " << std::string(data_buffer) << endl;
    server_port = get_sender_port(sender_socket_info);
    server_ip = get_sender_ip(sender_socket_info);
    cout << "Sender port: " << server_port << ". Sender IP: " << server_ip << endl;
    return true;
}

std::string Client::get_sender_ip(struct sockaddr_in sender) const
{
    char ip_str[INET6_ADDRSTRLEN];
    const char* ip =  inet_ntop(AF_INET, &(sender.sin_addr), ip_str, INET6_ADDRSTRLEN);
    return std::string(ip);
}

int Client::get_sender_port(struct sockaddr_in sender) const
{
    return htons(sender.sin_port);
}

// Credit: https://stackoverflow.com/a/3120382/13933174
std::string Client::get_public_ip() const
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "ERROR: Creating temp socket to get public ip" << endl;
    }

    const char* kGoogleDnsIp = "8.8.8.8";
    uint16_t kDnsPort = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));
    assert(err != -1);

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);
    assert(err != -1);

    char ip_buf[16];
    const char* p = inet_ntop(AF_INET, &name.sin_addr, ip_buf, sizeof(ip_buf));
    assert(p);

    close(sock);
    return std::string(ip_buf);
}