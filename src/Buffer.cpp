#include "Buffer.h"
#include <iostream>
#include <string>
#include <cassert>

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
void Buffer::clear() {
  buf.clear();
  readPos = 0;
  writePos = 0;
}

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



/**
 * @brief 获取缓冲区起始位置
 * @return 缓冲区起始位置
 */
char* Buffer::BeginPtr() {
  return &*buf.begin();
}

/**
 * @brief 获取缓冲区起始位置
 * @return 缓冲区起始位置
 */
const char* Buffer::BeginPtr() const {
  return &*buf.begin();
}

/**
 * @brief 获取可写入的位置
 * @return 可写入的位置
 */
char* Buffer::BeginWrite() {
    return BeginPtr() + writePos;
}

/**
 * @brief 获取可写入的位置 const
 * @return 可写入的位置 const
 */
const char* Buffer::BeginWriteConst() const {
    return BeginPtr() + writePos;
}

/**
 * @brief 获取可读取的位置
 * @return 可读取的位置
 */
const char* Buffer::Peek() const {
    return BeginPtr() + readPos;
}

/**
 * @brief 获取可读取的字节数
 * @return 可读取的字节数
 */
const size_t Buffer::ReadableBytes() const {
    return writePos - readPos;
}

/**
 * @brief 读取数据后，更新可读取的位置
 * @param len 读取的字节数
 */
void Buffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    readPos += len;
}

/**
 * @brief 读取数据直到某个位置
 * @param end 读取的位置
 */
void Buffer::RetrieveUntil(const char* end) {
    assert(Peek() <= end );
    Retrieve(end - Peek());
}