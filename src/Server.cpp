#include "Server.h"

#include <unistd.h>

#include <functional>
#include <iostream>
#include <thread>

#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

Server::Server(EventLoop* _loop) : main_reactor(_loop), acceptor(nullptr) {
    acceptor = new Acceptor(_loop);  // Acceptor由main_reactor负责
    std::function<void(Socket*)> cb = [this](Socket* socket) {
        this->new_connnection(socket);
    };
    acceptor->set_newconnection_callback(cb);

    int thread_size = std::thread::hardware_concurrency();  // 线程数量，也是sub_reactor数量
    thpool = new ThreadPool(thread_size);
    for (int i = 0; i < thread_size; i++) {
        sub_reactors.emplace_back(new EventLoop(i));
    }

    for (int i = 0; i < thread_size; i++) {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, sub_reactors[i]);
        thpool->add(sub_loop);
    }
    std::cout << "server start success" << std::endl;
}

Server::~Server() {
    delete acceptor;
    delete thpool;
}

void Server::new_connnection(Socket* _sock) {
    errif(_sock->get_fd() == -1, "new connection error");
    uint64_t rand = _sock->get_fd() % sub_reactors.size();  // 随机分配sub_reactor
    Connection* conn = new Connection(sub_reactors[rand], _sock);
    std::function<void(Socket*)> cb = [this](Socket* sock) {
        this->delete_connection(sock);
    };
    conn->set_delete_connection_callback(cb);
    conn->set_on_connect_callback(on_connect_callback);
    connections[_sock->get_fd()] = conn;
}

void Server::delete_connection(Socket* _sock) {
    int sockfd = _sock->get_fd();
    auto it = connections.find(sockfd);
    if (it != connections.end()) {
        Connection* conn = connections[sockfd];
        connections.erase(sockfd);
        delete conn;
        conn = nullptr;
    }
}

void Server::on_connect(std::function<void(Connection*)> fn) {
    on_connect_callback = std::move(fn);
}