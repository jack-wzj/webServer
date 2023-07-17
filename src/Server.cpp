#include "Server.h"
#include "Acceptor.h"
#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "util.h"
#include "ThreadPool.h"
#include <functional>
#include <unistd.h>
#include <iostream>

/**
 * @brief 构造函数
 * @details 构造Server对象，初始化接收器
 * @param _loop 事件循环对象
 */
Server::Server(EventLoop *_loop) : mainReactor(_loop), acceptor(nullptr), threadPool(nullptr) {
  acceptor = new Acceptor(mainReactor); // 接收器只由 mainReactor 管理
  // 将新连接的处理函数绑定到Acceptor的回调函数中
  std::function<void(Socket *)> cb =
      std::bind(&Server::newConnection, this, std::placeholders::_1);
  // 设置Acceptor的回调函数
  acceptor->setNewConnectionCallback(cb);

  // 初始化 subReactor
  int threadNum = std::thread::hardware_concurrency();
  threadPool = new ThreadPool(threadNum);
  for (int i = 0; i < threadNum; i++) {
    subReactors.push_back(new EventLoop()); // 每个线程是一个 subReactor
    // 将 subReactor 的事件循环函数绑定到线程池中
    std::function<void()> subLoop = std::bind(&EventLoop::loop, subReactors[i]);
    threadPool->addTask(std::move(subLoop)); // 开启所有线程的事件循环
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
  errif(client_sock->getFd() == -1, "Server::newConnection() error");
  // TODO: subReactor 调度算法有没有更好的?
  // 随机调度，选择一个 subReactor 处理新连接
  int random = client_sock->getFd() % subReactors.size();
  std::cout << "new connection, random " << random << std::endl;
  Connection *conn = new Connection(subReactors[random], client_sock);
  std::function<void(Socket *)> cb =
      std::bind(&Server::deleteConnection, this, std::placeholders::_1);
  conn->setDeleteConnectionCallback(cb);
  // 设置事务处理回调函数
  conn->SetOnConnectCallback(onConnCallback);
  connections[client_sock->getFd()] = conn;
}

/**
 * @brief 断开 TCP 连接
 * @details 由Connection调用
 * @param sock 客户端套接字
 */
void Server::deleteConnection(Socket *sock) {
  int sockfd = sock->getFd();
  auto it = connections.find(sockfd);
  if (it != connections.end()) {
    Connection *conn = it->second;
    connections.erase(sockfd);
    // close(sockfd);
    delete conn;
    conn = nullptr;
  }
}

/**
 * @brief 设置新连接的回调函数
 * @details Connection
 * @param cb 回调函数
 */
void Server::OnConnect(std::function<void(Connection *)> cb) {
  std::cout << "Server::OnConnect()" << std::endl;
  onConnCallback = std::move(cb);
}