/**
 * @file InetAddress.h
 * @brief InetAddress类的声明
 * @details 用于封装地址信息，主要维护sockaddr_in结构体
 */
#pragma once
#include <arpa/inet.h>

class InetAddress {
public:
    sockaddr_in addr;
    socklen_t addr_len;

    InetAddress();
    InetAddress(const char *ip, uint16_t port);
    ~InetAddress() = default;
};