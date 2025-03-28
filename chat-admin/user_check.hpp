// 가입된 회원 목록 조회

#pragma once
#include<iostream>
#include"httplib.h"
#include"DB_admin.hpp"
#include <mysql/jdbc.h>
#include <windows.h>
#include <string>
#include "json.hpp"

using namespace std;
using namespace sql;

// 회원 목록 조회 클래스 및 함수
class User_check {
private:
	Connection* conn;
public:
	User_check(Connection* connection) : conn(connection){}

	void User_check_func() {
		unique_ptr<Statement> stmt{ conn->createStatement() };
		unique_ptr<ResultSet> res{ stmt->executeQuery("SELECT user_id, user_name  FROM User") };
		while (res->next()) {
			cout << res->getString("user_id") << endl;
			cout << res->getString("user_name") << endl;
		}
	}
	void handle_user_check(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);
			User_check_func();
			json response = { req_json };
			res.set_content(response.dump(), "application/json");
			res.set_content("Check sucess", "text/plain");
		}
		catch (const SQLException& e) {
			cout << "Check failed" << e.what() << endl;
		}
	}
};