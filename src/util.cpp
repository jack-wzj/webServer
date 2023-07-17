#include "util.h"
#include <cstdio>
#include <cstdlib>
#include <string>

/**
 * @brief 错误处理函数
 * @param condition 条件
 * @param msg 错误信息
 */
void errif(bool condition, const char *msg) {
  if (condition) {
    perror(msg);
    exit(EXIT_FAILURE);
  }
}