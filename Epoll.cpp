#include <unistd.h>
#include <cstring>
#include "Epoll.h"
#include "util.h"
#include "Channel.h"

constexpr int MAX_EVENTS = 1024;

Epoll::Epoll() : epfd(-1), events(nullptr) {
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(epoll_event) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if (epfd != -1) {
        close(epfd);
        epfd= -1;
    }
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t op) {
    epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = op;
    ev.data.fd = fd;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl error");
}

std::vector<Channel *> Epoll::poll(int timeout) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll_wait error");
    std::vector<Channel *> ret;
    for (int i = 0; i < nfds; i++) {
        Channel *channel = (Channel *)events[i].data.ptr;
        channel->setRevents(events[i].events);
        ret.push_back(channel);
    }
    return ret;
}

void Epoll::updateChannel(Channel *channel) {
    int fd = channel->getFd();
    epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    if (!channel->isInEpoll()) { // 不在 epoll 中，添加
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->setInEpoll();
    }
    else { // 在 epoll 中，修改
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll mod error");
    }
}