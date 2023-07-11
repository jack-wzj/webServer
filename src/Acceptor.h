/**
 * @file Acceptor.h
 * @brief Acceptor类的声明
 * @details 声明Acceptor类，该类用于接收新连接
 */
#pragma once
#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();
    void acceptConnection();
    void setNewConnectionCallback(std::function<void(Socket*)>);
    
private:
    EventLoop *loop;
    Socket *sock;
    // InetAddress *addr;
    Channel *acceptChannel;
    std::function<void(Socket*)> newConnectionCallback;
};