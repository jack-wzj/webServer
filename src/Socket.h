/**
 * @file Socket.h
 * @brief Socket类，封装套接字
 * @details 创建套接字、绑定套接字、监听套接字、接受连接、设置非阻塞等操作
 */
#pragma once

class InetAddress;

class Socket {
public:
    Socket();
    Socket(int);
    ~Socket();

    int getFd();
    void bind(InetAddress *);
    int accept(InetAddress *);
    void listen();
    void connect(InetAddress *);
    void setNoBlocking();

private:
    int sockfd;
};
