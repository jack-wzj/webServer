/**
 * @file Epoll.h
 * @brief Epoll类的声明
 * @details 声明Epoll类，封装epoll相关操作
 */
#pragma once
#include <sys/epoll.h>
#include <vector>


class Channel;
class Epoll {
public:
    Epoll();
    ~Epoll();

    std::vector<Channel *> poll(int timeout = -1);  // 调用epoll_wait()，返回发生事件的所有channel
    void updateChannel(Channel *);  // 将channel添加到epoll中


private:
    int epfd;
    epoll_event *events;
};