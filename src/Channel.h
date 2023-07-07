#pragma once
#include <functional>
#include <sys/epoll.h>

class EventLoop;

class Channel {
public:
    Channel(EventLoop *loop, int fd);
    ~Channel();
    int getFd();
    void enableReading();
    uint32_t getEvents();
    uint32_t getRevents();
    bool isInEpoll();
    void setInEpoll();
    void setRevents(uint32_t);

    void handleEvent();
    void setCallback(std::function<void()>);


private:
    EventLoop *loop;
    int fd;
    uint32_t events;  // 监听事件
    uint32_t revents; // 发生的事件
    bool inEpoll;     // 是否在epoll中
    std::function<void()> callback;
};