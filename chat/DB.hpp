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
	unique_ptr<Connection> conn;
public:
	MySQLConnector(const string& serv, const string& user, const string& pw, const string& db)
		: server(serv), username(user), password(pw), database(db) {
		try {
			mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
			conn = unique_ptr<Connection>(driver->connect(server, username, password));
			conn->setSchema(database);
		}
		catch (SQLException& e) {
			cerr << "MySQL Connection Failed!!" << endl;
		}
	}
	Connection* getConnection() {
		return conn.get();
	}
	~MySQLConnector() {
	}
};

// mysql 연결함수
inline Connection* mysql_db_conn() {
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	Connection* conn = driver->connect(SERVER_IP, USERNAME, PASSWORD);
	conn->setSchema(DATABASE);
	return conn;
}