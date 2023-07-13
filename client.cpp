#include <cstring>
#include <iostream>
#include <unistd.h>
#include "src/util.h"
#include "src/Buffer.h"
#include "src/InetAddress.h"
#include "src/Socket.h"

using namespace std;

constexpr int READ_BUFFER_SIZE = 1024;

int main() {
    // 建立一个socket套接字，对外提供一个网络通信接口
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8099);
    sock->connect(addr);

    int sockfd = sock->getFd();

    // 读写缓冲区
    Buffer *sendBuf = new Buffer();
    Buffer *recvBuf = new Buffer();

    // 向服务器发送数据
    while (true) {
        // 读取用户输入
        sendBuf->getline();
        // 向服务器发送数据
        ssize_t write_bytes = write(sockfd, sendBuf->c_str(), sendBuf->size());
        if (write_bytes == -1) {
            cout << "socket disconnected, cant write" << endl;
            break;
        }
        // 读取服务器返回的数据
        int already_read = 0;
        char buf[1024];
        while (true) {
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if (read_bytes > 0) {
                recvBuf->append(buf, read_bytes);
                already_read += read_bytes;
            } else if (read_bytes == 0) {
                cout << "server disconnected" << endl;
                exit(EXIT_SUCCESS);
            }
            if (already_read >= sendBuf->size()) {
                cout << "server: " << recvBuf->c_str() << endl;
                break;
            }
        }
        recvBuf->clear();
    }
    delete addr;
    delete sock;
    return 0;
}