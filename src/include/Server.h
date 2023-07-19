/**
 * Server类
 * @brief 服务器类
 * @details 服务器类，用于管理TCP连接
 */
#pragma once
#include <map>
#include <vector>
#include <functional>
#include <cstdint>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class SqlConnectPool;

class Server {
public:
  Server(EventLoop *, const char *ip, uint16_t port, const char *user, char *passwd, const char * dbname);
  ~Server();

  void newConnection(Socket *); // 新建TCP连接, 由Acceptor调用
  void deleteConnection(Socket *sock);   // 断开TCP连接
  void OnConnect(std::function<void(Connection *)>);
  void NewConnect(std::function<void(Connection *)>);
  void OnMessage(std::function<void(Connection *)>);
  SqlConnectPool* getConnPool() { return m_connPool; }
  
private:
  EventLoop *mainReactor; // 只负责接受连接，分发给一个 subReactor
  std::vector<EventLoop *> subReactors; // 负责处理连接上的读写事件
  Acceptor *acceptor;                   // 接收器
  std::map<int, Connection *> connections; // TCP连接
  ThreadPool *threadPool;                  // 线程池
  std::function<void(Connection *)> onConnCallback;
  std::function<void(Connection *)> onMsgCallback;
  std::function<void(Connection *)> newConnallback;
  SqlConnectPool *m_connPool; // 数据库连接池
};