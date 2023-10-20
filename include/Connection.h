#pragma once
#include <functional>
#include <string>

class EventLoop;
class Socket;
class Channel;
class Buffer;
class Connection {
public:
    enum State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed
    };
    Connection(EventLoop* _loop, Socket* _sock);
    ~Connection();

    void read();
    void write();
    void close();
    const char* read_buffer_str();
    const char* send_buffer_str();
    void getline_send_buffer();

    void set_delete_connection_callback(const std::function<void(Socket*)>& callback);
    void set_on_connect_callback(const std::function<void(Connection*)>& callback);
    void set_send_buffer(const char* str);

    State get_state();
    Buffer* get_read_buffer();
    Buffer* get_send_buffer();
    Socket* get_socket();

private:
    EventLoop* loop;
    Socket* sock;
    Channel* channel{nullptr};

    State state{State::Invalid};

    Buffer* read_buffer{nullptr};
    Buffer* send_buffer{nullptr};

    std::function<void(Socket*)> delete_connection_callback;
    std::function<void(Connection*)> on_connect_callback;

    void read_nonblocked();
    void write_nonblocked();
    void read_blocked();
    void write_blocked();
};