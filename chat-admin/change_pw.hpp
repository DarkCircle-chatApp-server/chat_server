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
	void Change_PW_func(const string& ch_login_pw, const int& user_id) {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET login_pw = ? WHERE user_id = ?") };
		// user_id = ? 로 자신의 것으로만 login_pw를 UPDATE
		pstmt->setString(1, ch_login_pw);
		pstmt->setInt(2, user_id);
		pstmt->executeUpdate();
	}

	// 현재 비밀번호가 DB의 비밀번호와 일치여부
	bool Is_pw_equal_DB(const string& plogin_pw, const int& user_id) {
		
		//예외 처리
		if (!conn) {
			cerr << "Database connection is not initialized!" << endl;
			return -1;
		}
		try {
			// 데이터 베이스에서 해당 user_id의 login_pw를 가져와서
			unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("SELECT login_pw FROM User WHERE user_id = ?") };
			pstmt->setInt(1, user_id);
			unique_ptr<ResultSet> res{ pstmt->executeQuery() };

			// DB 데이터와 입력한 데이터 동일여부 확인하여 같으면 1
			if (res->next()) {
				string db_user_pw = res->getString("login_pw");
				return db_user_pw == plogin_pw;
			}
			// 다르면 0
			else {
				return false;
			}
		}
		// 예외 발생 시 false 반환
		catch (const SQLException& e) {
			cerr << "SQL Error: " << e.what() << endl;
		}
		return false;
	}

	// 기능 호출 함수
	void handle_Change_PW(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			string plogin_pw = req_json["plogin_pw"];
			int user_id = req_json["user_id"];
			string ch_login_pw = req_json["ch_login_pw"];

			if (Is_pw_equal_DB(plogin_pw, user_id) == 1) {
				Change_PW_func(ch_login_pw, user_id);
				res.set_content("Change sucess", "text/plain");
			}
			else {
				cout << "This PW is different from DB PW" << endl;
				res.set_content("Change failed", "text/plain");
			}
		}
		catch (const SQLException& e) {
			cout << "Change failed by exception" << e.what() << endl;
		}
	}
};