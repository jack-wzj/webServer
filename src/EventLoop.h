/**
 * @file EventLoop.h
 * @brief EventLoop类的声明
 * @details 事件驱动核心类，该类用于管理事件循环。主要管理一个 Epoll()对象，以及事件循环loop()。
 */
#pragma once
#include <functional>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void updateChannel(Channel *);
    void addThread(std::function<void()>);
    
private:
    Epoll *ep;
    ThreadPool *threadPool;
    bool quit;
};