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
	int user_id;
	Connection* conn;
public:
	User_ban(Connection* connection) : conn(connection) {}

	// �� ���
	void User_ban_func(const int& user_id) {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 3 WHERE user_id = ?") };					
		// user_id�� status 3(ä�ñ���)��
		pstmt->setInt(1, user_id);
		pstmt->executeUpdate();
	}
	// �� ���� ���
	void User_unban_func(const int& user_id) {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?") };					
		// user_id�� status 1(�Ϲݻ���)��
		pstmt->setInt(1, user_id);
		pstmt->executeUpdate();
	}

	// �� ó�� �Լ�
	void handle_user_ban(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			// json ������ ����
			int user_id = req_json["user_id"];

			// �� ó��
			User_ban_func(user_id);
			res.set_content("Ban sucess", "text/plain");		// ����ȣ��
		}
		catch (const SQLException& e) {
			cout << "Ban failed" << e.what() << endl;
		}
	}

	// �� ���� ó�� �Լ�
	void handle_user_unban(const httplib::Request& req, httplib::Response& res) {
		try {
			json req_json = json::parse(req.body);

			// json ������ ����
			int user_id = req_json["user_id"];

			// �� ���� ó��
			User_unban_func(user_id);
			res.set_content("Unban sucess", "text/plain");		// ����ȣ��
		}
		catch (const SQLException& e) {
			cout << "Unban failed" << e.what() << endl;
		}
	}
};

