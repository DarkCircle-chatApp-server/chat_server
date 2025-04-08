// �ڽ��� ��й�ȣ ���� ���

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

// ��й�ȣ ���� Ŭ���� ���� �� ����Լ�
class Change_PW {
private:
	string login_pw;
	int user_id;
	Connection* conn;
public:
	Change_PW(Connection* connection) : conn(connection){}

	// PW ���� ���
	void Change_PW_func(const string& ch_login_pw, const int& user_id) {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET login_pw = ? WHERE user_id = ?") };
		// user_id = ? �� �ڽ��� �����θ� login_pw�� UPDATE
		pstmt->setString(1, ch_login_pw);
		pstmt->setInt(2, user_id);
		pstmt->executeUpdate();
	}

	// ���� ��й�ȣ�� DB�� ��й�ȣ�� ��ġ����
	bool Is_pw_equal_DB(const string& plogin_pw, const int& user_id) {
		
		//���� ó��
		if (!conn) {
			cerr << "Database connection is not initialized!" << endl;
			return -1;
		}
		try {
			// ������ ���̽����� �ش� user_id�� login_pw�� �����ͼ�
			unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("SELECT login_pw FROM User WHERE user_id = ?") };
			pstmt->setInt(1, user_id);
			unique_ptr<ResultSet> res{ pstmt->executeQuery() };

			// DB �����Ϳ� �Է��� ������ ���Ͽ��� Ȯ���Ͽ� ������ 1
			if (res->next()) {
				string db_user_pw = res->getString("login_pw");
				return db_user_pw == plogin_pw;
			}
			// �ٸ��� 0
			else {
				return false;
			}
		}
		// ���� �߻� �� false ��ȯ
		catch (const SQLException& e) {
			cerr << "SQL Error: " << e.what() << endl;
		}
		return false;
	}

	// ��� ȣ�� �Լ�
	void handle_Change_PW(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			string plogin_pw = req_json["plogin_pw"];
			int user_id = req_json["user_id"];
			string ch_login_pw = req_json["ch_login_pw"];

			if (Is_pw_equal_DB(plogin_pw, user_id) == 1) {
				Change_PW_func(ch_login_pw, user_id);
				res.status = 200;
				res.set_content(R"({"success": true, "message": "비밀번호가 변경되었습니다."})", "application/json");
				//res.set_content("Change sucess", "text/plain");
			}
			else {
				cout << "This PW is different from DB PW" << endl;
				res.status = 400;
				res.set_content(R"({"success": false, "message": "현재 비밀번호가 일치하지 않습니다."})", "application/json");
				//res.set_content("Change failed", "text/plain");
			}
		}
		catch (const SQLException& e) {
			cout << "Change failed by exception" << e.what() << endl;
		}
	}
};