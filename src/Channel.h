/**
 * @file Channel.h
 * @brief Channel类的声明，该类用于管理 fd 的事件
 * @details 将 fd 和事件绑定，用于 epoll 事件监听。事件发生时，调用回调函数。
*/
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
    bool inEpoll;     // 是否在epoll中。Epoll::updateChannel()时判断，如果不在则add到epoll中，否则mod
    std::function<void()> callback; // 事件发生时的回调函数
};