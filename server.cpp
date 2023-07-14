#include "src/Server.h"
#include "src/EventLoop.h"

int main() {
    EventLoop *mainReactor = new EventLoop();
    Server *server = new Server(mainReactor);
    mainReactor->loop();
    return 0;
}
