#include "Acceptor.h"

#include <cstdio>
#include <iostream>

#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop) {
    sock = new Socket();
    InetAddress* addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);
    sock->listen();
    // sock->set_nonblock();

    accept_channel = new Channel(loop, sock->get_fd());
    std::function<void()> cb = [this]() {
        this->accept_connection();
    };
    accept_channel->set_read_callback(cb);
    accept_channel->enable_read();
    std::cout << "Acceptor listen port " << addr->get_port() << std::endl;
    delete addr;
}

Acceptor::~Acceptor() {
    delete sock;
    delete accept_channel;
}

void Acceptor::accept_connection() {
    InetAddress* clnt_addr = new InetAddress();
    Socket* clnt_sock = new Socket(sock->accept(clnt_addr));
    clnt_sock->set_nonblocked();
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->get_fd(), inet_ntoa(clnt_addr->get_addr().sin_addr), ntohs(clnt_addr->get_addr().sin_port));
    if (new_connection_callback) {
        new_connection_callback(clnt_sock);
    }
    delete clnt_addr;
}

void Acceptor::set_newconnection_callback(std::function<void(Socket*)> _cb) {
    new_connection_callback = _cb;
}