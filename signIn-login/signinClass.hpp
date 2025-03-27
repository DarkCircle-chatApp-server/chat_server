#include <iostream>
#include "httplib.h"
#include <string>
#include <mysql/jdbc.h>
#include "DBmodule.hpp"
using namespace std;
using namespace sql;

// c++에는 date 자료형이 없어서 따로 년-월-일 담을 구조체 생성
struct Birthdate {
	int year;
	int month;
	int day;
};

// 회원가입 데이터 담을 클래스
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
	Connection* conn;	// Connection 타입의 포인터 conn
public:
	SignIn(Connection* dbconn) : conn(dbconn) {		// 의존성 주입. MySQLConnector 객체로부터 주입받음

	}
	void showUsers() {
		unique_ptr<Statement> stmt{ conn->createStatement() };
		unique_ptr<ResultSet> res{ stmt->executeQuery("SELECT user_name FROM User") };
		while (res->next()) {
			cout << res->getString("user_name") << endl;
		}
	}
};