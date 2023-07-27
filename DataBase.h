#pragma once

#if defined(_WIN64)
#pragma comment(lib, "libmysql.lib")
#include "mysql.h"
#elif defined(__linux__)
#include "mysql/mysql.h"
#endif

#include <string>
#include <vector>
#include <memory>

class DataBase
{
private:
	std::shared_ptr <MYSQL> _mysql;
	bool _connection;
	void close();

public:
	DataBase();
	~DataBase();
	bool connect(std::string host, unsigned int port, std::string dbname, std::string user, std::string password);
	bool hasConnection();
	std::vector<std::string> queryResult(std::string queryText);
	void executeQueryWithoutResult(std::string queryText);
};

