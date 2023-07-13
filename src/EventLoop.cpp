#include <vector>
#include "Epoll.h"
#include "Channel.h"
#include "EventLoop.h"
#include "ThreadPool.h"

/**
 * @brief EventLoop类的构造函数
 * @details 初始化epoll对象
 */
EventLoop::EventLoop() : ep(nullptr), threadPool(nullptr), quit(false) {
    ep = new Epoll();
    threadPool = new ThreadPool();
}

/**
 * @brief EventLoop类的析构函数
 * @details 释放epoll对象
 */
EventLoop::~EventLoop() {
    delete ep;
    delete threadPool;
}

/**
 * @brief 事件循环
 * @details 调用epoll_wait()，返回发生事件的所有channel，然后调用channel的handleEvent()处理事件
 */
void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel *> channels = ep->poll();
        for (auto channel : channels) {
            channel->handleEvent();
        }
    }
}

/**
 * @brief 更新channel
 * @details 将channel添加到epoll中
 * @param channel 要更新的channel
 */
void EventLoop::updateChannel(Channel *channel) {
    ep->updateChannel(channel);
}

/**
 * @brief 添加线程
 * @details 添加线程到线程池
 * @param func 线程执行的函数
 */
void EventLoop::addThread(std::function<void()> func) {
    threadPool->addTask(func);
}