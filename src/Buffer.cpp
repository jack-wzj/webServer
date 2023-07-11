#include <string>
#include <cstdio>
#include "Buffer.h"

/**
 * @brief 将数据写入缓冲区
 * @param data 数据
 * @param len 数据长度
 */
void Buffer::append(const char *data, int len) {
    buf.append(data, len);
}

/**
 * @brief 获取缓冲区大小
 * @return 缓冲区大小
 */
ssize_t Buffer::size() {
    return buf.size();
}

/**
 * @brief 获取缓冲区数据
 * @return 缓冲区数据
 */
const char* Buffer::c_str() {
    return buf.c_str();
}

/**
 * @brief 清空缓冲区
 */
void Buffer::clear() {
    buf.clear();
}