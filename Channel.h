#pragma once
#include <sys/epoll.h>

class Epoll;

class Channel {
public:
    Channel(Epoll *ep, int fd);
    ~Channel();
    int getFd();
    void enableReading();
    uint32_t getEvents();
    uint32_t getRevents();
    bool isInEpoll();
    void setInEpoll();
    void setRevents(uint32_t);


private:
    Epoll *ep;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
};