#pragma once
#include <arpa/inet.h>

class InetAddress {
private:
    struct sockaddr_in addr {};

public:
    InetAddress();
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress();

    void set_InetAddr(sockaddr_in _addr);
    sockaddr_in get_addr();
    const char* get_ip();
    uint16_t get_port();
};
