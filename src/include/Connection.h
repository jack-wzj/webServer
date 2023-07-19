/**
 * @file Connection.h
 * @brief Connection类的声明
 * @details 声明Connection类，该类封装 TCP 连接，处理 TCP 连接的读写事件
 */
#pragma once
#include <functional>
#include <string>

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
public:
  // 连接状态
  enum State {
    Invalid = 1,
    Handshaking,
    Connected,
    Closed,
    Failed,
  };

  Connection(EventLoop *, Socket *);
  ~Connection();

  void Read();
  void Write();
  // 向连接发送数据 msg
  void Send(std::string msg);
  // 业务逻辑
  void Business();

  void setDeleteConnectionCallback(std::function<void(Socket *)> const &callback);
  void SetOnConnectCallback(std::function<void(Connection *)> const &callback);
  void SetOnMessageCallback(std::function<void(Connection *)> const &callback);
  
  State GetState();
  void Close();
  void SetSendBuffer(const char *str);
  Buffer *GetReadBuffer();
  const char *ReadBuffer();
  Buffer *GetSendBuffer();
  const char *SendBuffer();
  void GetlineSendBuffer();
  Socket *GetSocket();
  
private:
  EventLoop *loop;
  Socket *sock;
  Channel *channel =  nullptr;
  State state = State::Invalid;
  // 回调函数
  std::function<void(Socket *)> deleteConnCallback;
  std::function<void(Connection *)> onConnCallback;
  std::function<void(Connection *)> onMsgCallback;
  // 读写缓冲区
  Buffer *readBuffer = nullptr;
  Buffer *writeBuffer = nullptr;
  // 读写事件处理函数
  void ReadNonBlocking();
  void WriteNonBlocking();
  void ReadBlocking();
  void WriteBlocking();
};
