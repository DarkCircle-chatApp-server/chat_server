#pragma once
#include <iostream>
#include <mysql/jdbc.h>

using namespace std;
using namespace sql;

#define SERVER_IP	"127.0.0.1:3306"
#define USERNAME	"root"
#define PASSWORD	"12345"
#define DATABASE	"chat"

class MySQLConnector {
private:
	string server;
	string username;
	string password;
	string database;
	Connection* conn;
public:
	MySQLConnector(const string& serv, const string& user, const string& passwd, const string& db)
		: server(serv), username(user), password(passwd), database(db) {
		try {
			mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
			conn = driver->connect(server, username, password);
			conn->setSchema(database);
			conn->setClientOption("OPT_RECONNECT", "true");  // �ڵ� �翬�� ����
			cout << "MySQL Connection success" << endl;
		}
		catch (SQLException& e) {
			cerr << "MySQL Connection failed" << e.what() << endl;
		}
		catch (exception& e) {
			cerr << "Unknown error during MySQL connection: " << e.what() << endl;
		}
	}
	// ������ conn ��ȯ(�ٸ� ��ü���� MySQLConnector�� conn�� ����ϱ� ���� �������) 
	Connection* getConnection() {
		if (!conn) {
			throw runtime_error("Database connection is not initialized or has failed.");
		}
		return conn;
	}
	~MySQLConnector() {
		if (conn) {
			cout << "MySQL Disconnect" << endl;
			delete conn;
		}
	}
};