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
Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock) {
    channel = new Channel(loop, sock->getFd());
    channel->enableRead();   // 可读事件监听
    channel->useET();        // 边缘触发
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(cb);   // 设置回调函数
    channel->setUseThreadPool(true);
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
            // close(sockfd);
            deleteConnectionCallback(sockfd);
            break;
        }
        else if (read_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { // EAGAIN 表示数据已经读完
            printf("recv from client fd %d: %s\n", sockfd, readBuffer->c_str());
            errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "write error");
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
            printf("unexpected!\n");
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
