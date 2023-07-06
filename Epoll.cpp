#include <unistd.h>
#include <cstring>
#include "Epoll.h"
#include "util.h"

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

std::vector<epoll_event> Epoll::poll(int timeout) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll_wait error");
    return std::vector<epoll_event>(events, events + nfds);
}