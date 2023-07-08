#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <functional>
#include <cerrno>
#include "EventLoop.h"
#include "Socket.h"
#include "Server.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"

constexpr int READ_BUFFER_SIZE = 1024;

Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr) {
    acceptor = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server() {
    delete acceptor;
}

void Server::handleReadEvent(int sockfd) {
    char buffer[READ_BUFFER_SIZE];
    while (true) { // 非阻塞I/O一次读取buf大小的数据，直到全读完
        bzero(&buffer, sizeof(buffer));
        ssize_t read_bytes = read(sockfd, buffer, sizeof(buffer));
        if (read_bytes == 0) { // EOF 表示客户端关闭连接
            printf("EOF, client %d disconnected\n", sockfd);
            close(sockfd);
            break;
        }
        else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // EAGAIN 表示数据已经读完
            printf("read done.\n");
            break;
        }
        else if (read_bytes == -1 && errno == EINTR) { // 表示客户端正常中断，继续读
            printf("continue reading.\n");
            continue;
        }
        else if (read_bytes > 0) {
            printf("recv from client fd %d: %s\n", sockfd, buffer);
            write(sockfd, buffer, sizeof(buffer));
        }
        else {
            printf("unexpected!\n");
            break;
        }
    }
}

void Server::newConnection(Socket *serv_sock) {
    InetAddress *client_addr = new InetAddress();   // 没有 delete
    Socket *client_sock = new Socket(serv_sock->accept(client_addr));
    printf("new client fd %d IP: %s Port: %d\n", client_sock->getFd(), inet_ntoa(client_addr->addr.sin_addr), ntohs(client_addr->addr.sin_port));
    client_sock->setNoBlocking(); // 设置为非阻塞
    Channel *client_channel = new Channel(loop, client_sock->getFd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, client_sock->getFd());
    client_channel->setCallback(cb);
    client_channel->enableReading();
}