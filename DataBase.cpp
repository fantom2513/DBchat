#include <iostream>

#include "DataBase.h"
#include <memory>

bool DataBase::connect(std::string host, unsigned int port, std::string dbname, std::string user, std::string password)
{
	if (!mysql_real_connect(_mysql.get(), host.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, NULL, 0)) {
		// Если нет возможности установить соединение с БД выводим сообщение об ошибке
		std::cout << "Error: can't connect to database " << mysql_error(_mysql.get()) << std::endl;
		_connection = false;
	}
	else {
		// Если соединение успешно установлено выводим фразу — "Success!"
		std::cout << "Success!" << std::endl;

		_connection = true;
	}
	return _connection;
}

bool DataBase::hasConnection()
{
	return _connection;
}

std::vector<std::string> DataBase::queryResult(std::string queryText)
{
	std::vector<std::string> result;
	MYSQL_RES* res;
	MYSQL_ROW row;
	
	mysql_query(_mysql.get(), queryText.c_str());

	if (res = mysql_use_result(_mysql.get())) {
		while (row = mysql_fetch_row(res)) {
			for (int i = 0; i < mysql_num_fields(res); i++) {
				result.push_back(row[i]);
			}
		}
	}
	else
		std::cout << "Error MySql query " << mysql_error(_mysql.get()) << std::endl;

	mysql_reset_connection(_mysql.get());

	return result;
}

void DataBase::executeQueryWithoutResult(std::string queryText)
{
	if (mysql_query(_mysql.get(), queryText.c_str()))
		std::cout << "Error execute query \"" << queryText << "\" - " << mysql_error(_mysql.get()) << std::endl;
}

void DataBase::close()
{
	mysql_close(_mysql.get());
}

DataBase::DataBase()
{
	_connection = false;
	_mysql = std::make_shared<MYSQL>();
	mysql_init(_mysql.get());

	if (_mysql == NULL) {
		// Если дескриптор не получен — выводим сообщение об ошибке
		std::cout << "Error: can't create MySQL-descriptor" << std::endl;
	}
}

DataBase::~DataBase()
{
	close();
}
