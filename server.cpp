#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "util.h"

using namespace std;

void errif(bool, const char *);

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
    // 接受客户端连接
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));
    // accept()函数会阻塞，直到有客户端连接
    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
    errif(client_sockfd == -1, "accept error!");
    printf("new client fd %d IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    // 读取客户端发送的数据
    while (true) {
        char buffer[1024];
        bzero(&buffer, sizeof(buffer));
        ssize_t read_bytes = read(client_sockfd, buffer, sizeof(buffer));
        if (read_bytes == 0) {
            printf("client %d closed\n", client_sockfd);
            close(client_sockfd);
            break;
        }
        else if (read_bytes == -1) {
            close(client_sockfd);
            errif(true, "read error!");
        }
        else if (read_bytes > 0) {
            printf("recv: %s\n", buffer);
            write(client_sockfd, buffer, sizeof(buffer));
        }
    }
    close(sockfd);
    
    return 0;
}