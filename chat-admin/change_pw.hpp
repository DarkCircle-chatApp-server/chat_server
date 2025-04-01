// 자신의 비밀번호 변경 기능

#pragma once
#include<iostream>
#include"httplib.h"
#include"DB_admin.hpp"
#include<mysql/jdbc.h>
#include<windows.h>
#include<string>
#include"json.hpp"

using namespace std;
using namespace sql;
using json = nlohmann::json;

// 비밀번호 변경 클래스 생성 및 기능함수
class Change_PW {
private:
	string login_pw;
	int user_id;
	Connection* conn;
public:
	Change_PW(Connection* connection) : conn(connection){}

	// PW 변경 기능
	void Change_PW_func(const string& login_pw, const int& user_id) {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET login_pw = ? WHERE user_id = ?") };
		// user_id = ? 로 자신의 것으로만 login_pw를 UPDATE
		pstmt->setString(1, login_pw);
		pstmt->setInt(2, user_id);
		pstmt->executeUpdate();
	}

	// 기능 호출 함수
	void handle_Change_PW(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			string login_pw = req_json["login_pw"];
			int user_id = req_json["user_id"];
			
			Change_PW_func(login_pw, user_id);
			res.set_content("Change sucess", "text/plain");
		}
		catch (const SQLException& e) {
			cout << "Change failed" << e.what() << endl;
		}
	}
};