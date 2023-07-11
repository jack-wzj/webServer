#include "InetAddress.h"
#include <cstring>

/**
 * @brief 构造函数
 * @details 初始化addr_len为sockaddr_in结构体的大小，将addr置零
 */
InetAddress::InetAddress() :addr_len(sizeof(addr)) {
    bzero(&addr, sizeof(addr));
}

/**
 * @brief 构造函数
 * @details 初始化addr_len为sockaddr_in结构体的大小，将addr置零，设置addr的ip和port
 * @param ip ip地址
 * @param port 端口号
 */
InetAddress::InetAddress(const char *ip, uint16_t port) {
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    
    addr_len = sizeof(addr);
}

