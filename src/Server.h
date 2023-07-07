#pragma once

class EventLoop;
class Socket;

class Server {
public:
    Server(EventLoop *);
    ~Server();

    void handleReadEvent(int);
    void newConnection(Socket *);

private:
    EventLoop *loop;
};