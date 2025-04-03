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
	int user_id;
	Connection* conn;
public:
	User_ban(Connection* connection) : conn(connection) {}

	// 밴 기능
	void User_ban_func(const int& user_id) {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 3 WHERE user_id = ?") };					
		// user_id의 status 3(채팅금지)로
		pstmt->setInt(1, user_id);
		pstmt->executeUpdate();
	}
	// 밴 해제 기능
	void User_unban_func(const int& user_id) {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?") };					
		// user_id의 status 1(일반상태)로
		pstmt->setInt(1, user_id);
		pstmt->executeUpdate();
	}

	// 밴 처리 함수
	void handle_user_ban(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			// json 데이터 추출
			int user_id = req_json["user_id"];

			// 밴 처리
			User_ban_func(user_id);
			res.set_content("Ban sucess", "text/plain");		// 응답호출
		}
		catch (const SQLException& e) {
			cout << "Ban failed" << e.what() << endl;
		}
	}

	// 밴 해제 처리 함수
	void handle_user_unban(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			// json 데이터 추출
			int user_id = req_json["user_id"];

			// 밴 해제 처리
			User_unban_func(user_id);
			res.set_content("Unban sucess", "text/plain");		// 응답호출
		}
		catch (const SQLException& e) {
			cout << "Unban failed" << e.what() << endl;
		}
	}
};

