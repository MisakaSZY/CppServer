#pragma once
#include <sys/epoll.h>

#include <vector>

class Channel;
class Epoll {
private:
    int epfd;
    struct epoll_event *events;

public:
    Epoll();
    ~Epoll();

    // void add_fd(int fd, uint32_t op);
    void update_channel(Channel *);
    void delete_channel(Channel *);
    // std::vector<epoll_event> poll(int timeout = -1);
    std::vector<Channel *> poll(int timeout = -1);
};