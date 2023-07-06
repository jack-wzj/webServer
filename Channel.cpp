#include "Channel.h"
#include "util.h"
#include "Epoll.h"

Channel::Channel(Epoll *ep, int fd) : ep(ep), fd(fd), events(0), revents(0), inEpoll(false) {
    errif(ep == nullptr, "epoll instance is null");
    errif(fd < 0, "fd < 0");
}

Channel::~Channel() {
    ep = nullptr;
    fd = -1;
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
    ep->updateChannel(this);
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getRevents() {
    return revents;
}