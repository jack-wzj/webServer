#pragma once

class EventLoop;
class Socket;
class Acceptor;

class Server {
public:
    Server(EventLoop *);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket *);

private:
    EventLoop *loop;
    Acceptor *acceptor;
};