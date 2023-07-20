/**
 * @file Buffer.h
 * @brief Buffer类的声明
 * @details 封装的 string 缓冲区，主要用于读写数据
 */
#pragma once
#include <string>
#include <atomic>

class Buffer {
public:
  Buffer() = default;
  ~Buffer() = default;
  void append(const char *data, int len);
  ssize_t size();
  const char *c_str();
  void clear();
  void getline();
  void setBuf(const char *);

  char* BeginPtr();
  const char* BeginPtr() const;
  char* BeginWrite();
  const char* BeginWriteConst() const;
  const char* Peek() const;
  const size_t ReadableBytes() const;
  void Retrieve(size_t len);
  void RetrieveUntil(const char* end);

private:
  std::string buf;
  std::atomic<std::size_t> readPos;
  std::atomic<std::size_t> writePos;
};