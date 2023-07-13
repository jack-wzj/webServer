#include <functional>
#include <unistd.h>
#include "EventLoop.h"
#include "Socket.h"
#include "Server.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"

 /**
  * @brief 构造函数
  * @details 构造Server对象，初始化接收器
  * @param _loop 事件循环对象
  */
Server::Server(EventLoop *_loop) : mainReactor(_loop), acceptor(nullptr), threadPool(nullptr) {
    acceptor = new Acceptor(mainReactor);   // 接收器只由 mainReactor 管理
    // 将新连接的处理函数绑定到Acceptor的回调函数中
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    // 设置Acceptor的回调函数
    acceptor->setNewConnectionCallback(cb);

    // 初始化 subReactor
    int threadNum = std::thread::hardware_concurrency();
    threadPool = new ThreadPool(threadNum);
    for (int i=0; i<threadNum; i++) {
        subReactors.push_back(new EventLoop()); // 每个线程是一个 subReactor
        // 将 subReactor 的事件循环函数绑定到线程池中
        std::function<void()> subLoop = std::bind(&EventLoop::loop, subReactors[i]);
        threadPool->addTask(subLoop);   // 开启所有线程的事件循环
    }
}

/**
 * @brief 析构函数
 * @details 析构Server对象，释放接收器
 */
Server::~Server() {
    delete acceptor;
    delete threadPool;
}

/**
 * @brief 接受新 TCP 连接
 * @details 由Acceptor调用
 * @param client_sock 客户端套接字
 */
void Server::newConnection(Socket *client_sock) {
    if (client_sock->getFd() != -1) {
        // TODO: subReactor 调度算法有没有更好的?
        // 随机调度，选择一个 subReactor 处理新连接
        EventLoop *loop = subReactors[client_sock->getFd() % subReactors.size()];
        Connection *conn = new Connection(loop, client_sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        connections[client_sock->getFd()] = conn;
    }
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
            // close(sockfd);
            delete conn;  
        }
    }
}