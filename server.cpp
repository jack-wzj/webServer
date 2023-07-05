#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "util.h"

using namespace std;

void errif(bool, const char *);
constexpr int MAX_EVENTS = 10;
constexpr int READ_BUFFER_SIZE = 1024;

void setNoBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    errif(flags == -1, "fcntl error!");
    flags |= O_NONBLOCK;
    errif(fcntl(fd, F_SETFL, flags) == -1, "fcntl error!");
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
    // 服务器地址
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8099);
    
    // 将socket地址与文件描述符绑定
    errif(bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "bind error!");
    // 监听端口
    // cout << "listen on port 8099: " << SOMAXCONN << endl; // 4096
    errif(listen(sockfd, SOMAXCONN) == -1, "listen error!");

    // 创建epoll实例
    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll_create1 error!");
    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));
    bzero(&ev, sizeof(ev));
    ev.events = EPOLLIN | EPOLLET;  // 监听读事件，边缘触发
    setNoBlocking(sockfd);          // 设置非阻塞
    ev.data.fd = sockfd;    // 监听的文件描述符
    // 将监听的文件描述符加入epoll实例
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev) == -1, "epoll_ctl error!");
    
    // 不断监听epoll上的事件
    while (true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); // nfds 为就绪的文件描述符数量
        errif(nfds == -1, "epoll_wait error!");
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockfd) {  //发生事件的fd是服务器socket fd，表示有新客户端连接
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                bzero(&client_addr, sizeof(client_addr));
                int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
                errif(client_sockfd == -1, "accept error!");
                printf("new client fd %d IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setNoBlocking(client_sockfd);
                errif(epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &ev) == -1, "epoll_ctl error!");
            }
            else if (events[i].events & EPOLLIN) {  // 有数据可读
                char buffer[READ_BUFFER_SIZE];
                while (true) { // 非阻塞I/O一次读取buf大小的数据，直到全读完
                    bzero(&buffer, sizeof(buffer));
                    ssize_t read_bytes = read(events[i].data.fd, buffer, sizeof(buffer));
                    if (read_bytes == 0) { // EOF 表示客户端关闭连接
                        printf("EOF, client %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);
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
                        printf("recv from client fd %d: %s\n", events[i].data.fd, buffer);
                        write(events[i].data.fd, buffer, sizeof(buffer));
                    }
                    else {
                        printf("unexpected!\n");
                        break;
                    }
                }
            }
        }
    }
    close(sockfd);
    
    return 0;
}