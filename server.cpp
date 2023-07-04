#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

using namespace std;

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 服务器地址
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8099);
    // 将socket地址与文件描述符绑定
    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    // 监听端口
    cout << "listen on port 8099: " << SOMAXCONN << endl; // 4096
    listen(sockfd, SOMAXCONN);
    // 接受客户端连接
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));
    // accept()函数会阻塞，直到有客户端连接
    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
    printf("new client fd %d IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    return 0;
}