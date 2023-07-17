#include "Buffer.h"
#include <iostream>
#include <string>

/**
 * @brief 将数据写入缓冲区
 * @param data 数据
 * @param len 数据长度
 */
void Buffer::append(const char *data, int len) {
  for (int i = 0; i < len; ++i) {
    if (data[i] == '\0') {
      break;
    }
    buf.push_back(data[i]);
  }
}

/**
 * @brief 获取缓冲区大小
 * @return 缓冲区大小
 */
ssize_t Buffer::size() { return buf.size(); }

/**
 * @brief 获取缓冲区数据
 * @return 缓冲区数据
 */
const char *Buffer::c_str() { return buf.c_str(); }

/**
 * @brief 清空缓冲区
 */
void Buffer::clear() { buf.clear(); }

/**
 * @brief 读取一行数据到缓冲区
 */
void Buffer::getline() {
  buf.clear();
  std::getline(std::cin, buf);
}

/**
 * @brief 设置缓冲区数据
 * @param data 数据
 */
void Buffer::setBuf(const char *data) {
  buf.clear();
  buf.append(data);
}