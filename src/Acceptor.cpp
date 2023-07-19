#include "Acceptor.h"
#include "Channel.h"
#include "Socket.h"
#include <cstdio>

Acceptor::Acceptor(EventLoop *_loop, const char *ip, uint16_t port)
    : loop(_loop), sock(nullptr), acceptChannel(nullptr) {
  sock = std::unique_ptr<Socket>(new Socket());
  InetAddress *addr = new InetAddress(ip, port);
  sock->bind(addr);
  sock->listen();

  acceptChannel = std::unique_ptr<Channel>(new Channel(loop, sock->getFd()));
  std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
  acceptChannel->setReadCallback(cb);
  acceptChannel->enableRead();
  delete addr;
}

Acceptor::~Acceptor() {}

void Acceptor::acceptConnection() {
  InetAddress *client_addr = new InetAddress();
  Socket *client_sock = new Socket(sock->accept(client_addr));
  printf("new client fd %d IP: %s Port: %d\n", client_sock->getFd(),
         client_addr->getIP(), client_addr->getPort());
  client_sock->setNoBlocking(); // 设置为非阻塞
  newConnectionCallback(client_sock);
  delete client_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> const &cb) {
  newConnectionCallback = std::move(cb);
}