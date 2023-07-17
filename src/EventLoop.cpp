#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "ThreadPool.h"
#include <vector>

/**
 * @brief EventLoop类的构造函数
 * @details 初始化epoll对象
 */
EventLoop::EventLoop() : ep(nullptr), quit(false) { ep = new Epoll(); }

/**
 * @brief EventLoop类的析构函数
 * @details 释放epoll对象
 */
EventLoop::~EventLoop() { delete ep; }

/**
 * @brief 事件循环
 * @details
 * 调用epoll_wait()，返回发生事件的所有channel，然后调用channel的handleEvent()处理事件
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
void EventLoop::updateChannel(Channel *channel) { ep->updateChannel(channel); }