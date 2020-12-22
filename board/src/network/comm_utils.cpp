#include "comm_utils.h"

// Credit: https://stackoverflow.com/a/3120382/13933174
std::string CommUtil::get_public_ip()
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