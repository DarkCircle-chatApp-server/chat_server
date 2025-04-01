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
	void Change_PW_func(const string& login_pw, const int& user_id) {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET login_pw = ? WHERE user_id = ?") };
		// user_id = ? �� �ڽ��� �����θ� login_pw�� UPDATE
		pstmt->setString(1, login_pw);
		pstmt->setInt(2, user_id);
		pstmt->executeUpdate();
	}

	// ��� ȣ�� �Լ�
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