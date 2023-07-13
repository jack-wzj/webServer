#include <iostream>
#include <unistd.h>
#include <string.h>
#include <functional>
#include "src/util.h"
#include "src/Buffer.h"
#include "src/InetAddress.h"
#include "src/Socket.h"
#include "src/ThreadPool.h"

using namespace std;

void oneClient(int epochs, int wait){
    Socket *sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8099);
    sock->connect(addr);

    int sockfd = sock->getFd();

    Buffer *sendBuf = new Buffer();
    Buffer *recvBuf = new Buffer();

    sleep(wait);
    int count = 0;
    while(count < epochs){
        sendBuf->setBuf("Hello server! I'm client!");
        ssize_t write_bytes = write(sockfd, sendBuf->c_str(), sendBuf->size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        char buf[1024];    //这个buf大小无所谓
        while(true){
            bzero(&buf, sizeof(buf));
            ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
            if(read_bytes > 0){
                recvBuf->append(buf, read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){         //EOF
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendBuf->size()){
                printf("count: %d, server: %s\n", count++, recvBuf->c_str());
                break;
            } 
        }
        recvBuf->clear();
    }
    delete addr;
    delete sock;
}

int main(int argc, char *argv[]) {
    int threadNum = 100;
    int epochs = 100;
    int wait = 0;
    int o;
    const char *optstring = "t:m:w:";
    while ((o = getopt(argc, argv, optstring)) != -1) {
        switch (o) {
            case 't':
                threadNum = stoi(optarg);
                break;
            case 'm':
                epochs = stoi(optarg);
                break;
            case 'w':
                wait = stoi(optarg);
                break;
            case '?':
                printf("error optopt: %c\n", optopt);
                printf("error opterr: %d\n", opterr);
                break;
        }
    }

    ThreadPool *poll = new ThreadPool(threadNum);
    std::function<void()> func = std::bind(oneClient, epochs, wait);
    for(int i = 0; i < threadNum; ++i){
        poll->addTask(func);
    }
    delete poll;
    return 0;
}