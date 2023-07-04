#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>

using namespace std;

int main() {
    // 建立一个socket套接字，对外提供一个网络通信接口
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // 服务器地址
    struct sockaddr_in serv_addr;
    // 初始化
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族、IP地址和端口
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8099);
    // 连接服务器
    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    cout << "connect to server" << endl;
    return 0;
}