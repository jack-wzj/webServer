#pragma once
#include <sys/epoll.h>
#include <vector>

class Epoll {
public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t);
    std::vector<epoll_event> poll(int timeout = -1);


private:
    int epfd;
    epoll_event *events;
};