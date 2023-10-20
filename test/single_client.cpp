#include <Connection.h>
#include <Socket.h>

#include <iostream>

int main() {
    Socket *sock = new Socket();
    sock->connect("127.0.0.1", 8888);

    Connection *conn = new Connection(nullptr, sock);

    while (true) {
        conn->getline_send_buffer();
        conn->write();
        if (conn->get_state() == Connection::State::Closed) {
            conn->close();
            break;
        }
        conn->read();
        std::cout << "Message from server: " << conn->read_buffer_str() << std::endl;
    }

    delete conn;
    return 0;
}