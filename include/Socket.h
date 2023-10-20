#pragma once
#include <arpa/inet.h>

class InetAddress;
class Socket {
private:
    int fd;

public:
    Socket();
    explicit Socket(int);
    ~Socket();

    void bind(InetAddress*);
    void listen();
    int accept(InetAddress*);

    void connect(InetAddress*);
    void connect(const char* ip, uint16_t port);

    void set_nonblocked();
    bool is_nonblocked();
    int get_fd();
};