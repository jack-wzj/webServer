#include <vector>
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"

EventLoop::EventLoop() : ep(nullptr), quit(false) {
    ep = new Epoll();
}

EventLoop::~EventLoop() {
    delete ep;
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel *> channels = ep->poll();
        for (auto channel : channels) {
            channel->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel *channel) {
    ep->updateChannel(channel);
}