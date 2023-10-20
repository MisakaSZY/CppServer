#include "Socket.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstring>

#include "InetAddress.h"
#include "util.h"

Socket::Socket() : fd(-1) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}
Socket::Socket(int _fd) : fd(_fd) {
    errif(fd == -1, "socket create error");
}

Socket::~Socket() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress* _addr) {
    struct sockaddr_in tmp_addr = _addr->get_addr();
    errif(::bind(fd, (sockaddr*)&tmp_addr, sizeof(tmp_addr)) == -1, "socket bind error");
    _addr->set_InetAddr(tmp_addr);
}

void Socket::listen() {
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::set_nonblocked() {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

bool Socket::is_nonblocked() {
    return (fcntl(fd, F_GETFL) & O_NONBLOCK) != 0;
}

int Socket::accept(InetAddress* _addr) {
    int clnt_sockfd = -1;
    struct sockaddr_in tmp_addr {};
    socklen_t addr_len = sizeof(tmp_addr);
    // 在非阻塞情况下，循环等待新连接
    if (is_nonblocked()) {
        while (true) {
            clnt_sockfd = ::accept(fd, (sockaddr*)&tmp_addr, &addr_len);
            if (clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                continue;
            }
            if (clnt_sockfd == -1) {
                errif(true, "socket accept error");
            } else {
                break;
            }
        }
    } else {
        printf("222\n");
        clnt_sockfd = ::accept(fd, (sockaddr*)&tmp_addr, &addr_len);
        errif(clnt_sockfd == -1, "socket accept error");
    }
    _addr->set_InetAddr(tmp_addr);
    return clnt_sockfd;
}

void Socket::connect(InetAddress* _addr) {
    struct sockaddr_in tmp_addr = _addr->get_addr();
    socklen_t addr_len = sizeof(tmp_addr);
    errif(::connect(fd, (sockaddr*)&tmp_addr, addr_len) == -1, "socket connect error");
}

void Socket::connect(const char* ip, uint16_t port) {
    InetAddress* addr = new InetAddress(ip, port);
    connect(addr);
    delete addr;
}

int Socket::get_fd() {
    return fd;
}