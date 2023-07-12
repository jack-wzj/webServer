#include "Channel.h"
#include "util.h"
#include "EventLoop.h"
#include <unistd.h>

/**
 * @brief Channel类构造函数
 * @param _loop 事件循环
 * @param fd 文件描述符
 */
Channel::Channel(EventLoop *_loop, int fd) : loop(_loop), fd(fd), events(0), ready(0), inEpoll(false), useThreadPool(true) {
}

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
 * @param ready 发生的事件
 */
void Channel::setReady(uint32_t ready) {
    this->ready = ready;
}

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
uint32_t Channel::getEvents() {
    return events;
}

/**
 * @brief 获取发生的事件
 * @return 发生的事件
 */
uint32_t Channel::getReady() {
    return ready;
}

/**
 * @brief 事件发生时的回调函数
 */
void Channel::handleEvent() {
    if (ready & (EPOLLIN | EPOLLPRI)) {
        if (useThreadPool) {
            loop->addThread(readCallback);
        } else {
            readCallback();
        }
    }
    if (ready & EPOLLOUT) {
        if (useThreadPool) {
            loop->addThread(writeCallback);
        } else {
            writeCallback();
        }
    }
}

/**
 * @brief 设置事件发生的回调函数
 * @param callback 回调函数
 */
// void Channel::setCallback(std::function<void()> callback) {
//     this->callback = callback;
// }

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

/**
 * @brief 设置是否使用线程池
 * @param use 是否使用线程池
 */
void Channel::setUseThreadPool(bool use) {
    useThreadPool = use;
}