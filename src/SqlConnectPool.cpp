#include <mysql/mysql.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <list>
#include <pthread.h>
#include <iostream>
#include <condition_variable>
#include <mutex>

#include "SqlConnectPool.h"
#include "util.h"


SqlConnectPool::SqlConnectPool() {
	m_CurConn = 0;
	m_FreeConn = 0;
}

// 懒汉模式，静态局部变量方法实现线程安全
SqlConnectPool *SqlConnectPool::GetInstance() {
	static SqlConnectPool connPool;
	return &connPool;
}

// 构造初始化
void SqlConnectPool::init(const char *url, const char *User, const char *PassWord, const char *DBName, uint16_t Port, uint16_t MaxConn) 
{
    // 初始化数据库信息
    m_url = (char *)url;
    m_User = (char *)User;
    m_PassWord = (char *)PassWord;
    m_DatabaseName = (char *)DBName;
    m_Port = Port;
    
	// 创建MaxConn条数据库连接
	for (int i = 0; i < MaxConn; i++) {
		MYSQL *con = NULL;
		// 使用 mysql_init() 初始化连接
        errif((con = mysql_init(con)) == NULL, "MySQL init Error");

		// mysql_real_connect() 建立一个到mysql数据库的连接
		con = mysql_real_connect(con, url, User, PassWord, DBName, Port, NULL, 0);
        errif(con == NULL, "MySQL connect Error");

		// 更新连接池和空闲连接数量
		connList.push_back(con);
		++m_FreeConn;
	}

	// 同步信号量 赋值为最大连接次数
	m_MaxConn = m_FreeConn;
    printf("数据库连接池建立完成\n");
}


// 当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *SqlConnectPool::GetConnection()
{
	MYSQL *con = NULL;

	if (0 == connList.size())
		return NULL;
    // 根据条件变量判断是否有空闲连接，若没有则阻塞等待
    std::unique_lock<std::mutex> locker(m_cond);
    m_notEmpty.wait(locker, [this]{ 
        return m_FreeConn > 0;
    });
    // 互斥锁保护连接池
    {
        std::lock_guard<std::mutex> locker(m_lock);
        // 取连接池第一个元素用
        con = connList.front();
        connList.pop_front();
        // 修改状态值
        --m_FreeConn;
        ++m_CurConn;
    }
	return con;
}

//释放当前使用的连接
bool SqlConnectPool::ReleaseConnection(MYSQL *con)
{
	if (NULL == con)
		return false;
	{
        std::lock_guard<std::mutex> locker(m_lock);
        // 当前连接用完, 放回连接线程池以备重复利用
        connList.push_back(con);
        ++m_FreeConn;
        --m_CurConn;
    }
	return true;
}

//销毁数据库连接池
void SqlConnectPool::DestroyPool()
{
    {
        std::lock_guard<std::mutex> locker(m_lock);
        if (connList.size() > 0)
        {
            // 遍历连接池List, 挨个关闭连接
            std::list<MYSQL *>::iterator it;
            for (it = connList.begin(); it != connList.end(); ++it)
            {
                MYSQL *con = *it;
                mysql_close(con);	// 关闭连接
            }
            // 重置连接池参数
            m_CurConn = 0;
            m_FreeConn = 0;
            // 清空连接池
            connList.clear();
        }
    }
}

//当前空闲的连接数
int SqlConnectPool::GetFreeConn()
{
	return this->m_FreeConn;
}

// RAII机制销毁连接池
SqlConnectPool::~SqlConnectPool()
{
	DestroyPool();
}

connectionRAII::connectionRAII(MYSQL **SQL, SqlConnectPool *connPool){
	*SQL = connPool->GetConnection();
	
	conRAII = *SQL;
	poolRAII = connPool;
}

connectionRAII::~connectionRAII(){
	poolRAII->ReleaseConnection(conRAII);
}