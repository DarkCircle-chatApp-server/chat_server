#pragma once

#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <sw/redis++/redis++.h>

using namespace std;
using namespace sql;
using namespace sw::redis;

#define MYSQL_SERVER_IP		"127.0.0.1:3306"
#define MYSQL_USERNAME		"root"
#define MYSQL_PASSWORD		"12345"
#define MYSQL_DATABASE		"chat"
#define REDIS_SERVER_IP		"127.0.0.1"

#define REDIS_PORT			6379
#define REDIS_PASSWORD		"admin1234"

class MySQLConnector {				// MYSQL 연결 클래스
private:
	string server;
	string username;
	string password;
	string database;
	unique_ptr<sql::Connection> conn;
public:
	MySQLConnector(const string& serv, const string& user, const string& pw, const string& db)
		: server(serv), username(user), password(pw), database(db) {
		try {
			mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
			conn = unique_ptr<sql::Connection>(driver->connect(server, username, password));
			conn->setSchema(database);
		}
		catch (SQLException& e) {
			cerr << "MySQL Connection Failed!! " << e.what() << endl;
		}
	}
	sql::Connection* getConnection() {
		return conn.get();
	}
	~MySQLConnector() {
	}
};

// mysql 연결함수
inline unique_ptr<sql::Connection> mysql_db_conn() {
	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
		unique_ptr<sql::Connection> conn(driver->connect(MYSQL_SERVER_IP, MYSQL_USERNAME, MYSQL_PASSWORD));
		conn->setSchema(MYSQL_DATABASE);
		return conn;
	}
	catch (sql::SQLException& e) {
		cerr << "mysql 오류" << e.what() << endl;
		return nullptr;
	}
}

struct R_Conn {					// Redis 연결 구조체
	sw::redis::ConnectionOptions opts;

	R_Conn() {
		opts.host = REDIS_SERVER_IP;
		opts.port = REDIS_PORT;
		opts.password = REDIS_PASSWORD;
	}
};