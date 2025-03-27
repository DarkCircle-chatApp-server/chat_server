#include <iostream>
#include "httplib.h"
#include <string>
#include <mysql/jdbc.h>
#include "DBmodule.hpp"
using namespace std;
using namespace sql;

// c++���� date �ڷ����� ��� ���� ��-��-�� ���� ����ü ����
struct Birthdate {
	int year;
	int month;
	int day;
};

// ȸ������ ������ ���� Ŭ����
class SignIn {
private:
	string login_id;
	string login_pw;
	string user_name;
	string user_addr;
	string user_phone;
	int user_status;
	string user_email;
	Birthdate user_birthdate;
	Connection* conn;	// Connection Ÿ���� ������ conn
public:
	SignIn(Connection* dbconn) : conn(dbconn) {		// ������ ����. MySQLConnector ��ü�κ��� ���Թ���

	}
	void showUsers() {
		unique_ptr<Statement> stmt{ conn->createStatement() };
		unique_ptr<ResultSet> res{ stmt->executeQuery("SELECT user_name FROM User") };
		while (res->next()) {
			cout << res->getString("user_name") << endl;
		}
	}
};