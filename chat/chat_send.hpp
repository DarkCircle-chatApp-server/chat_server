#pragma once

#include <iostream>
#include <string>
#include "DB.hpp"

using namespace std;
using namespace sql;

class Chat_send {
private:
	int user_id;
	const int user_status = NULL;		// 유저 상태값 상수화 선언

	string msg_text;
	string msg_time;
	Connection* conn;
public:
	Chat_send(int _user_id, const char* _msg_text, const char* _msg_time, Connection* dbconn)
		: user_id(_user_id), msg_text(_msg_text), msg_time(_msg_time), conn(dbconn) {
	}

	void insert_chat() {		// 채팅 DB로 저장하는 함수
		if (user_status != 3) {			// 임시 차단이 아니면 아래 코드 실행
			cout << "채팅 입력 ";
			getline(cin, msg_text);		// 채팅 입력 받기

			try {		// 입력 받은 채팅 쿼리 함수로 DB에 추가
				unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("INSERT INTO Message (user_id, msg_text) VALUES(?, ?)"));
				pstmt->setInt(1, user_id);
				pstmt->setString(2, msg_text);
				pstmt->executeUpdate();
			}
			catch (SQLException& e) {		// 쿼리 연결 오류
				cout << "MySQL Connection Failed: " << e.what() << endl;
				cout << "SQL Error Code: " << e.getErrorCode() << endl;
				cout << "SQL State: " << e.getSQLState() << endl;
			}
		}
		else {
			cout << "차단 상태" << endl;
			stat_check();
		}
	}

	void stat_check() {			// 유저 상태(임시차단) 확인 함수
		int* num_fix = (int*)&user_status;

		try {
			unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("select U.user_id, U.user_status from User as U join Message as M on U.user_id = M.user_id where U.user_id = ? group by U.user_id"));
			pstmt->setInt(1, user_id);
			unique_ptr<ResultSet> res(pstmt->executeQuery());

			while (res->next()) {
				*num_fix = res->getInt("user_status");		// 클래스 내 유저 상태 상수값 변경
			}
		}
		catch (SQLException& e) {
			cout << "MySQL Connection Failed: " << e.what() << endl;
			cout << "SQL Error Code: " << e.getErrorCode() << endl;
			cout << "SQL State: " << e.getSQLState() << endl;
		}
	}

	void print_chat() {
		cout << msg_text << endl;
	}
};