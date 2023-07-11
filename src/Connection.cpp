#include <functional>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include "EventLoop.h"
#include "Socket.h"
#include "Connection.h"
#include "Channel.h"

// 缓冲区大小
constexpr int READ_BUFFER_SIZE = 1024;

/**
 * @brief Connection类的构造函数
 * @param _loop 事件循环对象
 * @param _sock Socket对象
 */
Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock) {
    channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setCallback(cb);   // 设置回调函数
    channel->enableReading();   // 可读事件监听
}

/**
 * @brief Connection类的析构函数
 */
Connection::~Connection() {
    delete channel;
    delete sock;
}

/**
 * @brief 请求处理函数：将客户端发送的数据原样返回
 * @param sockfd 客户端套接字描述符
 */
void Connection::echo(int sockfd) {
    char buffer[READ_BUFFER_SIZE];
    while (true) { // 非阻塞I/O一次读取buf大小的数据，直到全读完
        bzero(&buffer, sizeof(buffer));
        ssize_t read_bytes = read(sockfd, buffer, sizeof(buffer));
        if (read_bytes == 0) { // EOF 表示客户端关闭连接
            printf("EOF, client %d disconnected\n", sockfd);
            // close(sockfd);
            deleteConnectionCallback(sock);
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

/**
 * @brief 设置删除连接的回调函数
 * @param _cb 回调函数
 */
void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> _cb) {
    deleteConnectionCallback = _cb;
}