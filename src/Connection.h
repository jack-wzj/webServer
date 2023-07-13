/**
 * @file Connection.h
 * @brief Connection类的声明
 * @details 声明Connection类，该类封装 TCP 连接，处理 TCP 连接的读写事件
 */
#pragma once
#include <functional>

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
public:
    Connection() = delete;
    Connection(EventLoop *, Socket *);
    ~Connection();

    void echo(int);
    void setDeleteConnectionCallback(std::function<void(int)>);

    void send(int sockfd);

private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(int)> deleteConnectionCallback;
    Buffer *readBuffer;
};
