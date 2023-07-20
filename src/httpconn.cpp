#include "httpconn.h"
using namespace std;

const char* HttpConn::srcDir;             // HTTP服务器的根目录
bool HttpConn::isET;                      // 是否使用边缘触发模式

HttpConn::HttpConn(EventLoop *_loop, Socket *_sock): Connection(_loop, _sock) {
    isClose_ = true;
};

HttpConn::~HttpConn() { 
    Close(); 
};

void HttpConn::init(int fd, const sockaddr_in& addr) {
    assert(fd > 0);
    isClose_ = false;
    // LOG_INFO("Client[%d](%s:%d) in, userCount:%d", fd_, GetIP(), GetPort(), (int)userCount);
}

void HttpConn::Close() {
    response_.UnmapFile();              // 解除映射的文件
    if(isClose_ == false){
        isClose_ = true; 
        Connection::Close();
        // LOG_INFO("Client[%d](%s:%d) quit, UserCount:%d", fd_, GetIP(), GetPort(), (int)userCount);
    }
}


// 从套接字中读取数据到读缓冲区，并返回读取的字节数
ssize_t HttpConn::read(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = readBuffer->ReadFd(fd_, saveErrno);
        if (len <= 0) {
            break;
        }
    } while (isET);
    return len;
}

// 从写缓冲区将数据写入套接字，并返回写入的字节数
ssize_t HttpConn::write(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = writev(fd_, iov_, iovCnt_);
        if(len <= 0) {
            *saveErrno = errno;
            break;
        }
        if(iov_[0].iov_len + iov_[1].iov_len  == 0) { break; } /* 传输结束 */
        else if(static_cast<size_t>(len) > iov_[0].iov_len) {
            iov_[1].iov_base = (uint8_t*) iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len);
            if(iov_[0].iov_len) {
                writeBuffer->clear();
                iov_[0].iov_len = 0;
            }
        }
        else {
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len; 
            iov_[0].iov_len -= len; 
            writeBuffer->Retrieve(len);   // 从写缓冲区中删除已经写入套接字的数据
        }
    } while(isET || ToWriteBytes() > 10240);   // 达到写缓冲区的阈值或使用边缘触发模式时继续循环写入
    return len;
}

// 处理HTTP请求
bool HttpConn::process() {
    request_.Init();
    if(readBuffer->ReadableBytes() <= 0) {
        return false;
    }
    else if(request_.parse(readBuffer)) {
        // LOG_DEBUG("%s", request_.path().c_str());
        response_.Init(srcDir, request_.path(), request_.IsKeepAlive(), 200);
    } else {
        response_.Init(srcDir, request_.path(), false, 400);
    }

    // response_.MakeResponse(writeBuffer);   // 生成HTTP响应，并将响应数据存放到写缓冲区中

    /* 响应头 */
    iov_[0].iov_base = const_cast<char*>(writeBuffer->Peek());
    iov_[0].iov_len = writeBuffer->ReadableBytes();
    iovCnt_ = 1;

    /* 文件 */
    if(response_.FileLen() > 0  && response_.File()) {
        iov_[1].iov_base = response_.File();
        iov_[1].iov_len = response_.FileLen();
        iovCnt_ = 2;
    }
    
    // LOG_DEBUG("filesize:%d, %d  to %d", response_.FileLen() , iovCnt_, ToWriteBytes());
    return true;
}