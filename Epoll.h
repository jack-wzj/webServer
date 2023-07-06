#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll {
public:
    Epoll();
    ~Epoll();

    void addFd(int, uint32_t);
    std::vector<Channel *> poll(int timeout = -1);

    void updateChannel(Channel *);


private:
    int epfd;
    epoll_event *events;
};