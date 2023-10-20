#pragma once
#include <functional>
#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server {
private:
    EventLoop* main_reactor;
    Acceptor* acceptor;
    std::map<int, Connection*> connections;
    std::vector<EventLoop*> sub_reactors;
    ThreadPool* thpool;
    std::function<void(Connection*)> on_connect_callback;

public:
    explicit Server(EventLoop*);
    ~Server();

    void new_connnection(Socket* sock);    // 新建TCP连接
    void delete_connection(Socket* sock);  // 断开TCP连接
    void on_connect(std::function<void(Connection*)> fn);
};