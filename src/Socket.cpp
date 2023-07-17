#include "Socket.h"
#include "util.h"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

/**
 * @brief 构造函数
 * @details 创建套接字
 */
Socket::Socket() : sockfd(-1) {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errif(sockfd == -1, "socket create error");
}

/**
 * @brief 构造函数
 * @details 从已有的套接字创建
 * @param fd 已有的套接字
 */
Socket::Socket(int _fd) : sockfd(_fd) {
  errif(sockfd == -1, "socket create error");
}

/**
 * @brief 析构函数
 * @details 关闭套接字
 */
Socket::~Socket() {
  if (sockfd != -1) {
    close(sockfd);
    sockfd = -1;
  }
}

/**
 * @brief 绑定地址
 * @details 将套接字和地址绑定
 * @param serv_addr 地址
 */
void Socket::bind(InetAddress *serv_addr) {
  sockaddr_in addr = serv_addr->getAddr();
  errif(::bind(sockfd, (sockaddr *)&addr, sizeof(addr)) == -1, "bind error");
}

/**
 * @brief 接受连接
 * @details 接受客户端的连接
 * @param client_addr 客户端地址
 * @return 客户端的套接字
 */
int Socket::accept(InetAddress *client_addr) {
  int clientfd = -1;
  sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  socklen_t addr_len = sizeof(addr);
  if (fcntl(sockfd, F_GETFL) & O_NONBLOCK) {
    while (true) {
      clientfd = ::accept(sockfd, (sockaddr *)&addr, &addr_len);
      if (clientfd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        continue;
      } else if (clientfd == -1) {
        errif(true, "socket accept error");
      } else {
        break;
      }
    }
  } else {
    clientfd = ::accept(sockfd, (sockaddr *)&addr, &addr_len);
    errif(clientfd == -1, "socket accept error");
  }

  client_addr->setInetAddr(addr);
  return clientfd;
}

/**
 * @brief 监听
 * @details 监听套接字
 */
void Socket::listen() {
  errif(::listen(sockfd, SOMAXCONN) == -1, "listen error");
}

/**
 * @brief 连接
 * @details 连接服务器
 * @param serv_addr 服务器地址
 */
void Socket::connect(InetAddress *serv_addr) {
  sockaddr_in addr = serv_addr->getAddr();
  if (fcntl(sockfd, F_GETFL) & O_NONBLOCK) {
    while (true) {
      int ret = ::connect(sockfd, (sockaddr *)&addr, sizeof(addr));
      if (ret == 0) {
        break;
      } else if (ret == -1 && (errno == EINPROGRESS)) {
        continue;
      } else if (ret == -1) {
        errif(true, "socket connect error");
      }
    }
  } else {
    errif(::connect(sockfd, (sockaddr *)&addr, sizeof(addr)) == -1,
          "socket connect error");
  }
}

/**
 * @brief 设置非阻塞
 * @details 设置套接字为非阻塞模式
 */
void Socket::setNoBlocking() {
  fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
}

/**
 * @brief 获取套接字
 * @details 获取套接字 fd
 * @return 套接字
 */
int Socket::getFd() { return sockfd; }

/**
 * @brief 构造函数
 * @details 初始化addr_len为sockaddr_in结构体的大小，将addr置零
 */
InetAddress::InetAddress() { bzero(&addr, sizeof(addr)); }

/**
 * @brief 构造函数
 * @details
 * 初始化addr_len为sockaddr_in结构体的大小，将addr置零，设置addr的ip和port
 * @param ip ip地址
 * @param port 端口号
 */
InetAddress::InetAddress(const char *ip, uint16_t port) {
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
}

/**
 * @brief 设置地址
 * @param _addr 地址
 */
void InetAddress::setInetAddr(sockaddr_in _addr) { addr = _addr; }

/**
 * @brief 获取地址
 * @return 地址
 */
sockaddr_in InetAddress::getAddr() { return addr; }

/**
 * @brief 获取ip地址
 * @return ip地址
 */
char *InetAddress::getIP() { return inet_ntoa(addr.sin_addr); }

/**
 * @brief 获取端口号
 * @return 端口号
 */
uint16_t InetAddress::getPort() { return ntohs(addr.sin_port); }