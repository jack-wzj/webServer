#pragma once
#include <mysql/mysql.h>
#include <map>
#include <string>
#include <mutex>

class SqlConnectPool;

class Game {
public:
    Game();
    ~Game();
    void initmysql_result(SqlConnectPool *);
    // 注册
    int Register(std::string, std::string);
    // 登录
    int Login(std::string, std::string);

private:
    MYSQL *mysql;
    // 保存用户名和密码
    static std::map<std::string, std::string> users;
    // 保证线程安全
    std::mutex m_lock;
};

