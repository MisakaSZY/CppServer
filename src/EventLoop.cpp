#include "EventLoop.h"

#include <vector>

#include "Channel.h"
#include "Epoll.h"

EventLoop::EventLoop(int _loopid) : ep(nullptr), quit(false), loopid(_loopid) {
    ep = new Epoll();
}

EventLoop::~EventLoop() {
    delete ep;
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*> chs;
        chs = ep->poll();
        for (auto it = chs.begin(); it != chs.end(); it++) {
            (*it)->handle_event();
        }
    }
}

void EventLoop::update_channel(Channel* ch) {
    ep->update_channel(ch);
}

int EventLoop::get_loopid() {
    return loopid;
}