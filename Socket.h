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
    void setNoBlocking();

private:
    int sockfd;
};
