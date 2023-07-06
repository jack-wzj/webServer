#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "util.h"
#include "InetAddress.h"
#include "Socket.h"

void errif(bool, const char *);

Socket::Socket() : sockfd(-1){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
}

Socket::Socket(int fd) : sockfd(fd) {
    errif(sockfd == -1, "socket create error");
}

Socket::~Socket() {
    if (sockfd != -1) {
        close(sockfd);
        sockfd = -1;
    }
}

void Socket::bind(InetAddress *serv_addr) {
    errif(::bind(sockfd, (sockaddr*)&serv_addr->addr, serv_addr->addr_len) == -1, "bind error");
}

int Socket::accept(InetAddress *client_addr) {
    int connfd = ::accept(sockfd, (sockaddr*)&client_addr->addr, &client_addr->addr_len);
    errif(connfd == -1, "accept error");
    return connfd;
    // printf("new client fd %d IP: %s Port: %d\n", connfd, inet_ntoa(client_addr->getAddr().sin_addr), ntohs(client_addr->getAddr().sin_port));
}

void Socket::listen() {
    errif(::listen(sockfd, SOMAXCONN) == -1, "listen error");
}

void Socket::setNoBlocking() {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
}

int Socket::getFd() {
    return sockfd;
}

