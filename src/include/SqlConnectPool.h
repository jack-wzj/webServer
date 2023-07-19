#pragma once
#include <mysql/mysql.h>
#include <mutex>
#include <condition_variable>
#include <list>


class SqlConnectPool {
public:
	MYSQL *GetConnection();				 //获取数据库连接
	bool ReleaseConnection(MYSQL *conn); //释放连接
	int GetFreeConn();					 //获取空闲连接数
	void DestroyPool();					 //销毁所有连接

	//单例模式
	static SqlConnectPool *GetInstance();

	void init(const char *url, const char *User, const char *PassWord, const char *DataBaseName, uint16_t Port, uint16_t MaxConn); 

private:
	SqlConnectPool();
	~SqlConnectPool();

	uint16_t m_MaxConn;  //最大连接数
	uint16_t m_CurConn;  //当前已使用的连接数
	uint16_t m_FreeConn; //当前空闲的连接数

    std::mutex m_cond, m_lock;  // 互斥锁
    std::condition_variable m_notEmpty; // 条件变量
    std::list<MYSQL *> connList; //连接池

public:
	char * m_url;		 //主机地址
	uint16_t m_Port;		 //数据库端口号
	char * m_User;		 //登陆数据库用户名
	char * m_PassWord;	 //登陆数据库密码
	char * m_DatabaseName; //使用数据库名
};

class connectionRAII{

public:
	// 二级指针对指针进行修改
	connectionRAII(MYSQL **con, SqlConnectPool *connPool);
	~connectionRAII();
	
private:
	MYSQL *conRAII;
	SqlConnectPool *poolRAII;
};