#include <functional>
#include "EventLoop.h"
#include "Socket.h"
#include "Server.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"

 /**
  * @brief 构造函数
  * @details 构造Server对象，初始化接收器
  * @param _loop 事件循环对象
  */
Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr) {
    acceptor = new Acceptor(loop);
    // 将新连接的处理函数绑定到Acceptor的回调函数中
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    // 设置Acceptor的回调函数
    acceptor->setNewConnectionCallback(cb);
}

/**
 * @brief 析构函数
 * @details 析构Server对象，释放接收器
 */
Server::~Server() {
    delete acceptor;
}

/**
 * @brief 接受新 TCP 连接
 * @details 由Acceptor调用
 * @param client_sock 客户端套接字
 */
void Server::newConnection(Socket *client_sock) {
    Connection *conn = new Connection(loop, client_sock);
    std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connections[client_sock->getFd()] = conn;
}

/**
 * @brief 断开 TCP 连接
 * @details 由Connection调用
 * @param sock 客户端套接字
 */
void Server::deleteConnection(int sockfd) {
    if (sockfd != -1) {
        auto it = connections.find(sockfd);
        if (it != connections.end()) {
            Connection *conn = it->second;
            connections.erase(it);
            delete conn;
        }
    }
}