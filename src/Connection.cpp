#include <cerrno>
#include <cstdio>
#include <cstring>
#include <functional>
#include <unistd.h>
#include <cassert>

#include "Connection.h"
#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include "util.h"

/**
 * @brief Connection类的构造函数
 * @param _loop 事件循环对象
 * @param _sock Socket对象
 */
Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock) {
  if (loop != nullptr) {
    channel = new Channel(loop, sock->getFd());
    channel->enableRead(); // 可读事件监听
    channel->useET();      // 边缘触发
  }
  readBuffer = new Buffer();
  writeBuffer = new Buffer();
  state = State::Connected;
}

/**
 * @brief Connection类的析构函数
 */
Connection::~Connection() {
  if (loop != nullptr) {
    delete channel;
  }
  delete sock;
  delete readBuffer;
  delete writeBuffer;
}

/**
 * @brief 读取数据到缓冲区
 */
void Connection::Read() {
  errif(state != State::Connected, "Connection::Read() state != Connected");
  readBuffer->clear();
  if (sock->isNoBlocking()) {
    ReadNonBlocking();
  } else {
    ReadBlocking();
  }
}

/**
 * @brief 从缓冲区写入数据
 */
void Connection::Write() {
  errif(state != State::Connected, "Connection::Write() state != Connected");
  if (sock->isNoBlocking()) {
    WriteNonBlocking();
  } else {
    WriteBlocking();
  }
  writeBuffer->clear();
}

/**
 * @brief 设置删除连接的回调函数
 * @param _cb 回调函数
 */
void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> const &_cb) {
  deleteConnCallback = _cb;
}

/**
 * @brief 设置事务处理的回调函数
 * @param _cb 回调函数
 */
void Connection::SetOnConnectCallback(std::function<void(Connection *)> const &_cb) {
  onConnCallback = _cb;
  channel->setReadCallback([this]() { onConnCallback(this); });
}

/**
 * @brief 非阻塞读数据
 * @details 使用非阻塞IO，一次读取buf大小数据，直到全部读取完毕
 */
void Connection::ReadNonBlocking() {
  int sockfd = sock->getFd();
  char buf[1024];  // 这个buf大小无所谓
  while (true) {   // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
    memset(buf, 0, sizeof(buf));
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      readBuffer->append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {  // 程序正常中断、继续读取
      printf("continue reading\n");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("read EOF, client fd %d disconnected\n", sockfd);
      state = State::Closed;
      break;
    } else {
      printf("Other error on client fd %d\n", sockfd);
      state = State::Closed;
      break;
    }
  }
}
/**
 * @brief 非阻塞写数据
*/
void Connection::WriteNonBlocking() {
  int sockfd = sock->getFd();
  char buf[writeBuffer->size()];
  memcpy(buf, writeBuffer->c_str(), writeBuffer->size());
  int data_size = writeBuffer->size();
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EINTR) {
      printf("continue writing\n");
      continue;
    }
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    if (bytes_write == -1) {
      printf("Other error on client fd %d\n", sockfd);
      state = State::Closed;
      break;
    }
    data_left -= bytes_write;
  }
}

/**
 * @brief 阻塞读数据
 * @details 只有客户端可能调用
 */
void Connection::ReadBlocking() {
  int sockfd = sock->getFd();
  unsigned int rcv_size = 0;
  socklen_t len = sizeof(rcv_size);
  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);
  char buf[rcv_size];
  ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
  if (bytes_read > 0) {
    readBuffer->append(buf, bytes_read);
  } else if (bytes_read == 0) {
    printf("read EOF, blocking client fd %d disconnected\n", sockfd);
    state = State::Closed;
  } else if (bytes_read == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state = State::Closed;
  }
}

/**
 * @brief 阻塞写数据
 * @details 只有客户端可能调用
 */
void Connection::WriteBlocking() {
  // 没有处理send_buffer_数据大于TCP写缓冲区，的情况，可能会有bug
  int sockfd = sock->getFd();
  ssize_t bytes_write = write(sockfd, writeBuffer->c_str(), writeBuffer->size());
  if (bytes_write == -1) {
    printf("Other error on blocking client fd %d\n", sockfd);
    state = State::Closed;
  }
}

void Connection::Close() {
    deleteConnCallback(sock);
}

Connection::State Connection::GetState() {
   return state; 
}

void Connection::SetSendBuffer(const char *str) {
    writeBuffer->setBuf(str);
}

Buffer *Connection::GetReadBuffer() {
    return readBuffer;
}

const char *Connection::ReadBuffer() {
    return readBuffer->c_str();
}

Buffer *Connection::GetSendBuffer() {
    return writeBuffer;
}

const char *Connection::SendBuffer() {
    return writeBuffer->c_str();
}

void Connection::GetlineSendBuffer() {
    writeBuffer->getline();
}

Socket *Connection::GetSocket() {
    return sock;
}