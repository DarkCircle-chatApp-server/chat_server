// ä�� �� ���

#pragma once
#include<iostream>
#include"httplib.h"
#include"DB_admin.hpp"
#include <mysql/jdbc.h>
#include <windows.h>
#include <string>

using namespace std;
using namespace sql;

class User_ban {
private:
	string user_id;
	Connection* conn;
public:
	User_ban(Connection* connection) : conn(connection) {}

	void set_user_id(const string& id) {
		user_id = id;
	}
	void User_ban_func() {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 3 WHERE user_id = ?") };
		// "user_id = 1" �� �κ��� Ŭ���� �ش� user_id�� ������ ��
		pstmt->setString(1, user_id);
		pstmt->executeUpdate();
	}
	void User_unban_func() {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?") };
		// "user_id = 1" �� �κ��� Ŭ���� �ش� user_id�� ������ ��
		pstmt->setString(1, user_id);
		pstmt->executeUpdate();
	}
};