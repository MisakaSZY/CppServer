#include "InetAddress.h"

#include <cstring>

InetAddress::InetAddress() {}

InetAddress::InetAddress(const char* ip, uint16_t port) {
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
}

InetAddress::~InetAddress() {}

void InetAddress::set_InetAddr(sockaddr_in _addr) {
    addr = _addr;
}

sockaddr_in InetAddress::get_addr() {
    return addr;
}

const char* InetAddress::get_ip() {
    return inet_ntoa(addr.sin_addr);
}

uint16_t InetAddress::get_port() {
    return ntohs(addr.sin_port);
}