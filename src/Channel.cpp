#include "Channel.h"
#include "EventLoop.h"
#include "util.h"
#include <cstdio>
#include <unistd.h>

/**
 * @brief Channel类构造函数
 * @param _loop 事件循环
 * @param _fd 文件描述符
 */
Channel::Channel(EventLoop *_loop, int _fd)
    : loop(_loop), fd(_fd), events(0), ready(0), inEpoll(false) {}

/**
 * @brief Channel类析构函数
 */
Channel::~Channel() {
  if (fd != -1) {
    close(fd);
    fd = -1;
  }
}

/**
 * @brief 获取文件描述符
 * @return 文件描述符
 */
int Channel::getFd() { return fd; }

/**
 * @brief 设置已在epoll中
 */
void Channel::setInEpoll(bool _in) { inEpoll = _in; }

/**
 * @brief 判断是否在epoll中
 * @return 是否在epoll中
 */
bool Channel::isInEpoll() { return inEpoll; }

/**
 * @brief 设置发生的事件
 * @param ready 发生的事件
 */
void Channel::setReady(uint32_t _rd) { ready = _rd; }

/**
 * @brief 设置监听事件为可读
 */
void Channel::enableRead() {
  events |= EPOLLIN | EPOLLPRI;
  loop->updateChannel(this);
}

/**
 * @brief 设置监听事件以ET模式
 */
void Channel::useET() {
  events |= EPOLLET;
  loop->updateChannel(this);
}

/**
 * @brief 获取监听事件
 * @return 监听事件
 */
uint32_t Channel::getEvents() { return events; }

/**
 * @brief 获取发生的事件
 * @return 发生的事件
 */
uint32_t Channel::getReady() { return ready; }

/**
 * @brief 事件发生时的回调函数
 */
void Channel::handleEvent() {
  if (ready & (EPOLLIN | EPOLLPRI)) {
    readCallback();
  } else if (ready & EPOLLOUT) {
    writeCallback();
  } else {
    printf("unexpected handleEvent! %d %d %d\n", ready & EPOLLIN,
           ready & EPOLLPRI, ready);
  }
}

/**
 * @brief 设置读事件发生的回调函数
 * @param callback 回调函数
 */
void Channel::setReadCallback(std::function<void()> callback) {
  this->readCallback = callback;
}

/**
 * @brief 设置写事件发生的回调函数
 * @param callback 回调函数
 */
void Channel::setWriteCallback(std::function<void()> callback) {
  this->writeCallback = callback;
}