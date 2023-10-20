#pragma once
#include <sys/epoll.h>

#include <functional>

class EventLoop;
class Channel {
private:
    EventLoop* loop;
    int fd;

    uint32_t events;
    uint32_t ready;

    bool in_epoll;

    std::function<void()> read_callback;
    std::function<void()> write_callback;

public:
    Channel(EventLoop* _loop, int _fd);
    ~Channel();

    void enable_read();
    void handle_event();

    int get_fd();
    void use_et();

    uint32_t get_events();
    uint32_t get_ready();
    bool get_inepoll();
    void set_inepoll(bool in = true);
    void set_ready(uint32_t);
    void set_read_callback(std::function<void()>);
};
