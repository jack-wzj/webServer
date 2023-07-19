#include "Game.h"
#include "SqlConnectPool.h"
#include "util.h"
#include <string>
#include <cstdio>
#include <cstring>
#include <mutex>

// 初始化静态成员变量
std::map<std::string, std::string> Game::users;

Game::Game()
{
    mysql = NULL;
}

Game::~Game()
{
    mysql_close(mysql);
}

void Game::initmysql_result(SqlConnectPool *sqlPool)
{
    //先从连接池中取一个连接
    MYSQL *sqlConn = NULL;
    connectionRAII mysqlcon(&sqlConn, sqlPool);
    
    //在user表中检索username，passwd数据，浏览器端输入
    int ret = mysql_query(sqlConn, "SELECT name,password FROM user");
    if (ret) {
        printf("SELECT error:%s\n", mysql_error(sqlConn));
    }

    //从表中检索完整的结果集
    MYSQL_RES *result = mysql_store_result(sqlConn);
    //返回结果集中的列数
    // int num_fields = mysql_num_fields(result);
    //返回所有字段结构的数组
    // MYSQL_FIELD *fields = mysql_fetch_fields(result);
    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while (MYSQL_ROW row = mysql_fetch_row(result)) {
        std::string temp1(row[0]);
        std::string temp2(row[1]);
        users[temp1] = temp2;
        printf("username:%s passwd:%s\n", temp1.c_str(), temp2.c_str());
    }
}

int Game::Register(std::string name, std::string passwd)
{
    //先检测用户名是否存在
    if (users.find(name) != users.end()) {
        return -1;
    }
    //不存在，就可以注册，先插入数据库
    char sql_insert[200];
    memset(sql_insert, 0, sizeof(sql_insert));
    sprintf(sql_insert, "INSERT INTO user(name, password) VALUES('%s', '%s')", name.c_str(), passwd.c_str());
    {
        std::lock_guard<std::mutex> locker(m_lock);
        int ret = mysql_query(mysql, sql_insert);
        if (ret != 0) {
            printf("INSERT error:%s\n", mysql_error(mysql));
            return -1;
        }
        users.insert(std::pair<std::string, std::string>(name, passwd));
    }
    //注册成功
    return 0;
}

int Game::Login(std::string name, std::string passwd)
{
    //先检测用户名是否存在
    if (users.find(name) == users.end()) {
        return -1;
    }
    //存在，检测密码是否正确
    else if (users[name] == passwd) {
        return 0;
    }
    return -1;
}