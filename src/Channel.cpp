#include "Channel.h"
#include "util.h"
#include "EventLoop.h"

Channel::Channel(EventLoop *_loop, int fd) : loop(_loop), fd(fd), events(0), revents(0), inEpoll(false) {
}

Channel::~Channel() {
}

int Channel::getFd() {
    return fd;
}

void Channel::setInEpoll() {
    inEpoll = true;
}

bool Channel::isInEpoll() {
    return inEpoll;
}

void Channel::setRevents(uint32_t revents) {
    this->revents = revents;
}

void Channel::enableReading() {
    events = EPOLLIN | EPOLLET;
    loop->updateChannel(this);
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getRevents() {
    return revents;
}

void Channel::handleEvent() {
    callback();
}

void Channel::setCallback(std::function<void()> callback) {
    this->callback = callback;
}