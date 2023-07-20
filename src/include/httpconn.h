#pragma once

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <errno.h>      
#include "SqlConnectPool.h"
#include "Buffer.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"

#include "httprequest.h"
#include "httpresponse.h"

class HttpConn : public Connection {
public:
    HttpConn(EventLoop *, Socket *);
    
    ~HttpConn();

    void init(int sockFd, const sockaddr_in& addr);

    ssize_t read(int* saveErrno);

    ssize_t write(int* saveErrno);

    void Close();

    int GetFd() const;

    int GetPort() const;

    const char* GetIP() const;
    
    sockaddr_in GetAddr() const;
    
    bool process();

    int ToWriteBytes() { 
        return iov_[0].iov_len + iov_[1].iov_len; 
    }

    bool IsKeepAlive() const {
        return request_.IsKeepAlive();
    }

    static bool isET;
    static const char* srcDir;
    
private:
    bool isClose_;
    
    int iovCnt_;
    struct iovec iov_[2];

    HttpRequest request_;
    HttpResponse response_;
};