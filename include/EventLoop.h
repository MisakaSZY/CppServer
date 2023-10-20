#pragma once
#include <functional>
class Epoll;
class Channel;
class ThreadPool;
class EventLoop {
private:
    Epoll *ep;
    bool quit;
    int loopid;

public:
    EventLoop(int _loopid = -1);
    ~EventLoop();

    void loop();
    void update_channel(Channel *);

    int get_loopid();

    // void add_thread(std::function<void()>);
};