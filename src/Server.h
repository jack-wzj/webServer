/**
 * Server类
 * @brief 服务器类
 * @details 服务器类，用于管理TCP连接
 */
#pragma once
#include <map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server {
public:
    Server(EventLoop *);
    ~Server();

    void newConnection(Socket *);   // 新建TCP连接, 由Acceptor调用
    void deleteConnection(int);// 断开TCP连接

private:
    EventLoop *loop;    // 事件循环
    Acceptor *acceptor; // 接收器
    std::map<int, Connection*> connections; // TCP连接
};