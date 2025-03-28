// 채팅 밴 기능

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
using json = nlohmann::json;

// 밴 클래스 생성 및 기능 함수
class User_ban {
private:
	string user_id;
	Connection* conn;
public:
	User_ban(Connection* connection) : conn(connection) {}

	void set_user_id(const string& id) {
		user_id = id;
	}
	// 밴 기능
	void User_ban_func() {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 3 WHERE user_id = ?") };
		// "user_id = 1" 의 부분은 클릭시 해당 user_id로 변경할 것
		pstmt->setString(1, user_id);
		pstmt->executeUpdate();
	}
	// 밴 해제 기능
	void User_unban_func() {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?") };
		// "user_id = 1" 의 부분은 클릭시 해당 user_id로 변경할 것
		pstmt->setString(1, user_id);
		pstmt->executeUpdate();
	}

	// 밴 처리 함수
	void handle_user_ban(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			// json 데이터 추출
			string user_id = req_json["user_id"];
			int user_status = req_json["user_status"];

			// 밴 처리
			User_ban_func();
			res.set_content("Ban sucess", "text/plain");
		}
		catch (const SQLException& e) {
			cout << "login failed" << e.what() << endl;
		}
	}

	// 밴 해제 처리 함수
	void handle_user_unban(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			// json 데이터 추출
			string user_id = req_json["user_id"];
			int user_status = req_json["user_status"];

			// 밴 해제 처리
			User_unban_func();
			res.set_content("Unban sucess", "text/plain");
		}
		catch (const SQLException& e) {
			cout << "login failed" << e.what() << endl;
		}
	}
};

