// 채팅 밴 기능

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
		// "user_id = 1" 의 부분은 클릭시 해당 user_id로 변경할 것
		pstmt->setString(1, user_id);
		pstmt->executeUpdate();
	}
	void User_unban_func() {
		unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?") };
		// "user_id = 1" 의 부분은 클릭시 해당 user_id로 변경할 것
		pstmt->setString(1, user_id);
		pstmt->executeUpdate();
	}
};