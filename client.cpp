#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "src/util.h"

using namespace std;

void errif(bool, const char *);
constexpr int READ_BUFFER_SIZE = 1024;

int main() {
    // 建立一个socket套接字，对外提供一个网络通信接口
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");
    // 服务器地址
    struct sockaddr_in serv_addr;
    // 初始化
    bzero(&serv_addr, sizeof(serv_addr));
    // 设置地址族、IP地址和端口
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8099);

    // 连接服务器
    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "connect error!" );
    cout << "connect to server" << endl;

    string buf;
    // 向服务器发送数据
    while (true) {
        // 缓冲区初始化
        char buffer[READ_BUFFER_SIZE]; // buffer 大小要大于等于服务器的缓冲区大小，否则会出现粘包问题
        bzero(&buffer, sizeof(buffer));
        // 读取用户输入
        getline(cin, buf);
        // scanf("%s", buffer);
        // 向服务器发送数据
        ssize_t write_bytes = write(sockfd, buf.c_str(), buf.size());
        if (write_bytes == -1) {
            cout << "socket disconnected, cant write" << endl;
            break;
        }
        // 读取服务器返回的数据
        bzero(&buffer, sizeof(buffer));
        ssize_t read_bytes = read(sockfd, buffer, sizeof(buffer));
        if (read_bytes == -1) {
            close(sockfd);
            errif(true, "read error!");
        }
        else if (read_bytes == 0) {
            close(sockfd);
            cout << "server closed" << endl;
            break;
        }
        else if (read_bytes > 0) {
            cout << "recv: " << buffer << endl;
        }
        buf.clear();
    }
    close(sockfd);
    return 0;
}