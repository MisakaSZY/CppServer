#include "Server.h"

#include <iostream>

#include "Buffer.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"

int main() {
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);
    server->on_connect([](Connection* conn) {
        conn->read();
        if (conn->get_state() == Connection::State::Closed) {
            conn->close();
            return;
        }
        std::cout << "Message from client " << conn->get_socket()->get_fd() << ": " << conn->read_buffer_str() << std::endl;
        conn->set_send_buffer(conn->read_buffer_str());
        conn->write();
    });
    loop->loop();
    delete server;
    delete loop;
    return 0;
}