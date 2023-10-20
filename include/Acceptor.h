#pragma once
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
private:
    EventLoop* loop;
    Socket* sock;
    Channel* accept_channel;
    std::function<void(Socket*)> new_connection_callback;

public:
    Acceptor(EventLoop* _loop);
    ~Acceptor();

    void accept_connection();
    void set_newconnection_callback(std::function<void(Socket*)>);
};