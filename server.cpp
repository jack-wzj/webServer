#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "util.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Epoll.h"
#include "Channel.h"

using namespace std;

void handleReadEvent(int);

constexpr int MAX_EVENTS = 10;
constexpr int READ_BUFFER_SIZE = 1024;

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8099);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    // 创建epoll实例
    Epoll *ep = new Epoll();
    serv_sock->setNoBlocking();
    // 将监听的文件描述符加入epoll实例
    // ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);
    Channel *serv_channel = new Channel(ep, serv_sock->getFd());
    serv_channel->enableReading();
    
    // 不断监听epoll上的事件
    while (true) {
        vector<Channel *> activeChannels = ep->poll();
        int nfds = activeChannels.size(); // nfds 为就绪的文件描述符数量
        for (int i = 0; i < nfds; i++) {
            if (activeChannels[i]->getFd() == serv_sock->getFd()) {  //发生事件的fd是服务器socket fd，表示有新客户端连接
                InetAddress *client_addr = new InetAddress();   // 没有 delete
                Socket *client_sock = new Socket(serv_sock->accept(client_addr));
                printf("new client fd %d IP: %s Port: %d\n", client_sock->getFd(), inet_ntoa(client_addr->addr.sin_addr), ntohs(client_addr->addr.sin_port));
                client_sock->setNoBlocking(); // 设置为非阻塞
                Channel *client_channel = new Channel(ep, client_sock->getFd());
                client_channel->enableReading();
                // ep->addFd(client_sock->getFd(), EPOLLIN | EPOLLET); // 将新连接的客户端socket fd加入epoll实例
            }
            else if (activeChannels[i]->getRevents() & EPOLLIN) {  // 有数据可读
                handleReadEvent(activeChannels[i]->getFd());
            }
            else {
                printf("unexpected!\n");
            }
        }
    }
    return 0;
}


void handleReadEvent(int sockfd) {
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