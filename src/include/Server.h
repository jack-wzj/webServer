/**
 * Server类
 * @brief 服务器类
 * @details 服务器类，用于管理TCP连接
 */
#pragma once
#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;

class Server {
public:
  Server(EventLoop *);
  ~Server();

  void newConnection(Socket *); // 新建TCP连接, 由Acceptor调用
  void deleteConnection(int);   // 断开TCP连接

private:
  EventLoop *mainReactor; // 只负责接受连接，分发给一个 subReactor
  std::vector<EventLoop *> subReactors; // 负责处理连接上的读写事件
  Acceptor *acceptor;                   // 接收器
  std::map<int, Connection *> connections; // TCP连接
  ThreadPool *threadPool;                  // 线程池
};