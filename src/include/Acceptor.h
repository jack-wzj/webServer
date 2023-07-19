/**
 * @file Acceptor.h
 * @brief Acceptor类的声明
 * @details 声明Acceptor类，该类用于接收新连接
 */
#pragma once
#include <functional>
#include <cstdint>
#include <memory>

class EventLoop;
class Socket;
class Channel;

class Acceptor {
public:
  Acceptor(EventLoop *_loop, const char *ip, uint16_t port);
  ~Acceptor();
  void acceptConnection();
  void setNewConnectionCallback(std::function<void(Socket *)> const &);

private:
  EventLoop *loop;
  std::unique_ptr<Socket> sock;
  std::unique_ptr<Channel> acceptChannel;
  std::function<void(Socket *)> newConnectionCallback;
};