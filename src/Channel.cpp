#include "Channel.h"
#include "util.h"
#include "EventLoop.h"

/**
 * @brief Channel类构造函数
 * @param _loop 事件循环
 * @param fd 文件描述符
 */
Channel::Channel(EventLoop *_loop, int fd) : loop(_loop), fd(fd), events(0), revents(0), inEpoll(false) {
}

/**
 * @brief Channel类析构函数
 */
Channel::~Channel() {
}

/**
 * @brief 获取文件描述符
 * @return 文件描述符
 */
int Channel::getFd() {
    return fd;
}

/**
 * @brief 设置已在epoll中
 */
void Channel::setInEpoll() {
    inEpoll = true;
}

/**
 * @brief 判断是否在epoll中
 * @return 是否在epoll中
 */
bool Channel::isInEpoll() {
    return inEpoll;
}

/**
 * @brief 设置发生的事件
 * @param revents 发生的事件
 */
void Channel::setRevents(uint32_t revents) {
    this->revents = revents;
}

/**
 * @brief 设置监听事件为可读
 */
void Channel::enableReading() {
    events = EPOLLIN | EPOLLET;
    loop->updateChannel(this);
}

/**
 * @brief 获取监听事件
 * @return 监听事件
 */
uint32_t Channel::getEvents() {
    return events;
}

/**
 * @brief 获取发生的事件
 * @return 发生的事件
 */
uint32_t Channel::getRevents() {
    return revents;
}

/**
 * @brief 事件发生时的回调函数
 */
void Channel::handleEvent() {
    loop->addThread(callback);
    // callback();
}

/**
 * @brief 设置事件发生的回调函数
 * @param callback 回调函数
 */
void Channel::setCallback(std::function<void()> callback) {
    this->callback = callback;
}