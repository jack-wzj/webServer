/**
 * @file Buffer.h
 * @brief Buffer类的声明
 * @details 封装的 string 缓冲区，主要用于读写数据
 */
#pragma once
#include <string>

class Buffer {
public:
    Buffer() = default;
    ~Buffer() = default;
    void append(const char *data, int len);
    ssize_t size();
    const char* c_str();
    void clear();
private:
    std::string buf;
};