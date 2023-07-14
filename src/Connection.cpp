#include <functional>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include "EventLoop.h"
#include "Socket.h"
#include "Connection.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"

/**
 * @brief Connection类的构造函数
 * @param _loop 事件循环对象
 * @param _sock Socket对象
 */
Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr), readBuffer(nullptr) {
    channel = new Channel(loop, sock->getFd());
    channel->enableRead();   // 可读事件监听
    channel->useET();        // 边缘触发
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(cb);   // 设置回调函数
    readBuffer = new Buffer();
}

/**
 * @brief Connection类的析构函数
 */
Connection::~Connection() {
    delete channel;
    delete sock;
    delete readBuffer;
}

/**
 * @brief 请求处理函数：将客户端发送的数据原样返回
 * @param sockfd 客户端套接字描述符
 */
void Connection::echo(int sockfd) {
    char buffer[1024];
    while (true) { // 非阻塞I/O一次读取buf大小的数据，直到全读完
        bzero(&buffer, sizeof(buffer));
        ssize_t read_bytes = read(sockfd, buffer, sizeof(buffer));
        if (read_bytes == 0) { // EOF 表示客户端关闭连接
            printf("EOF, client %d disconnected\n", sockfd);
            deleteConnectionCallback(sockfd);
            break;
        }
        else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // EAGAIN 表示数据已经读完
            printf("client fd %d: %s\n", sockfd, readBuffer->c_str());
            // errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "write error");
            send(sockfd);
            readBuffer->clear();
            break;
        }
        else if (read_bytes == -1 && errno == EINTR) { // 表示客户端正常中断，继续读
            printf("continue reading.\n");
            continue;
        }
        else if (read_bytes > 0) {
            readBuffer->append(buffer, read_bytes);
        }
        else {
            printf("Connection reset!\n");
            deleteConnectionCallback(sockfd);
            break;
        }
    }
}

/**
 * @brief 设置删除连接的回调函数
 * @param _cb 回调函数
 */
void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb) {
    deleteConnectionCallback = _cb;
}

void Connection::send(int sockfd) {
    char buf[readBuffer->size()];
    strcpy(buf, readBuffer->c_str());
    int data_size = readBuffer->size();
    int already_write = 0;
    while (already_write < data_size) {
        ssize_t write_bytes = write(sockfd, buf + already_write, data_size - already_write);
        if (write_bytes == -1 && errno == EAGAIN) {
            break;
        }
        already_write += write_bytes;
    }
}