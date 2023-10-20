#include "Channel.h"

#include <unistd.h>

#include "EventLoop.h"

Channel::Channel(EventLoop* _loop, int _fd)
    : loop(_loop), fd(_fd), events(0), ready(0), in_epoll(false) {}

Channel::~Channel() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Channel::handle_event() {
    if (ready & (EPOLLIN | EPOLLPRI)) {
        read_callback();
    }
    if (ready & (EPOLLOUT)) {
        write_callback();
    }
}

void Channel::enable_read() {
    events |= EPOLLIN | EPOLLPRI;
    loop->update_channel(this);
}

void Channel::use_et() {
    events |= EPOLLET;
    loop->update_channel(this);
}

int Channel::get_fd() {
    return fd;
}

uint32_t Channel::get_events() {
    return events;
}

uint32_t Channel::get_ready() {
    return ready;
}

bool Channel::get_inepoll() {
    return in_epoll;
}

void Channel::set_inepoll(bool in) {
    in_epoll = in;
}

void Channel::set_ready(uint32_t _ev) {
    ready = _ev;
}

void Channel::set_read_callback(std::function<void()> _cb) {
    read_callback = _cb;
}