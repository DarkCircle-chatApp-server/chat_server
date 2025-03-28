// ä�� �� ���

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

// �� Ŭ���� ���� �� ��� �Լ�
class User_ban {
private:
	string user_id;
	Connection* conn;
public:
	User_ban(Connection* connection) : conn(connection) {}

	void set_user_id(const string& id) {
		user_id = id;
	}
	// �� ���
	void User_ban_func() {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 3 WHERE user_id = ?") };
		// "user_id = 1" �� �κ��� Ŭ���� �ش� user_id�� ������ ��
		pstmt->setString(1, user_id);
		pstmt->executeUpdate();
	}
	// �� ���� ���
	void User_unban_func() {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?") };
		// "user_id = 1" �� �κ��� Ŭ���� �ش� user_id�� ������ ��
		pstmt->setString(1, user_id);
		pstmt->executeUpdate();
	}

	// �� ó�� �Լ�
	void handle_user_ban(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			// json ������ ����
			string user_id = req_json["user_id"];
			int user_status = req_json["user_status"];

			// �� ó��
			User_ban_func();
			res.set_content("Ban sucess", "text/plain");
		}
		catch (const SQLException& e) {
			cout << "login failed" << e.what() << endl;
		}
	}

	// �� ���� ó�� �Լ�
	void handle_user_unban(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			// json ������ ����
			string user_id = req_json["user_id"];
			int user_status = req_json["user_status"];

			// �� ���� ó��
			User_unban_func();
			res.set_content("Unban sucess", "text/plain");
		}
		catch (const SQLException& e) {
			cout << "login failed" << e.what() << endl;
		}
	}
};

