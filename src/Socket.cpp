#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "util.h"
#include "InetAddress.h"
#include "Socket.h"

/**
 * @brief 构造函数
 * @details 创建套接字
 */
Socket::Socket() : sockfd(-1){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
}

/**
 * @brief 构造函数
 * @details 从已有的套接字创建
 * @param fd 已有的套接字
 */
Socket::Socket(int fd) : sockfd(fd) {
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
    errif(::bind(sockfd, (sockaddr*)&serv_addr->addr, serv_addr->addr_len) == -1, "bind error");
}

/**
 * @brief 接受连接
 * @details 接受客户端的连接
 * @param client_addr 客户端地址
 * @return 客户端的套接字
 */
int Socket::accept(InetAddress *client_addr) {
    int connfd = ::accept(sockfd, (sockaddr*)&client_addr->addr, &client_addr->addr_len);
    errif(connfd == -1, "accept error");
    return connfd;
    // printf("new client fd %d IP: %s Port: %d\n", connfd, inet_ntoa(client_addr->getAddr().sin_addr), ntohs(client_addr->getAddr().sin_port));
}

/**
 * @brief 监听
 * @details 监听套接字
 */
void Socket::listen() {
    errif(::listen(sockfd, SOMAXCONN) == -1, "listen error");
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
int Socket::getFd() {
    return sockfd;
}

