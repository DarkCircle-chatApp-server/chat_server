#pragma once

#include "DB.hpp"
#include <ctime>

static int room_msg_num = 1;			// 채팅방 채팅 번호(갯수)
static int outp_msg_id = 1;				// redis에서 빼와야할 채팅 msg_id

class Chat_send {
private:
	const int user_id = 0;				// 유저 id 상수화 선언 (객체 생성시 고정 아이디 값)
	int local_msg_id;					// 접속한 객체(계정)가 부여받는 msg_id 값

	int r_all_msg = 0;					// redis에서 삭제해야할 채팅 갯수
	int user_status = 0;				// 유저 상태값 상수화 선언 (디버그 모드일때 오류나서 일단 상수X)

	string msg_text;		// 메시지 내용
	string msg_time;		// 타임 로그

	string redis_to_mysql[3] = {};			// redis에 저장된 데이터 sql로 옮기는 배열

	sql::Connection* m_conn;			// SQL 의존성 주입 객체
	shared_ptr<Redis> redis;			// Redis 의존성 주입 객체

	void current_date() {			// C++에서 현재 시간 생성을 MySQL TIMEDATE형에 변환한 함수(클래스 내 호출)
		time_t now = time(0);
		struct tm t;


		if (localtime_s(&t, &now) == 0) {
			string c_year = to_string((t.tm_year + 1900));
			string c_mon = to_string((t.tm_mon + 1));
			string c_day = to_string(t.tm_mday);
			string c_hour = to_string(t.tm_hour);
			string c_min = to_string(t.tm_min);
			string c_sec = to_string(t.tm_sec);

			string* time_format[] = { &c_mon, &c_day, &c_hour, &c_min, &c_sec };

			for (int i = 0; i < 5; i++) {
				if ((time_format[i]->length()) < 2) {
					*time_format[i] = "0" + *time_format[i];
				}
			}
			string c_date = c_year + "-" + c_mon + "-" + c_day + " " + c_hour + ":" + c_min + ":" + c_sec;

			msg_time = c_date;			// msg_time에 저장
		}
	}

	void stat_check() {			// 유저 상태(임시차단) 확인 함수
		int* num_fix = (int*)&user_status;

		try {
			unique_ptr<PreparedStatement> pstmt(m_conn->prepareStatement("select U.user_id, U.user_status from User as U join Message as M on U.user_id = M.user_id where U.user_id = ? group by U.user_id"));		// user_status 확인 쿼리문
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

	void del_chat_redis() {				// redis 데이터 삭제 함수
		try {
			string ins_m_id_redis = "msg_id:" + to_string(r_all_msg);		// msg_id 동적으로 입력받기
			redis->del(ins_m_id_redis);

			cout << "삭제 성공" << endl;
		}
		catch (const Error& e) {
			cerr << "Rediis 연결 오류: " << e.what() << endl;
		}
	}

	void out_chat_data_redis() {			// redis 데이터 빼오기
		try {
			int index_f = 0;				// redis_to_mysql 배열에 동적으로 넣기 위한 인덱스

			string ins_m_id_redis = "msg_id:" + to_string(outp_msg_id);		// msg_id 동적으로 입력받기

			std::vector<OptionalString> fields;

			redis->hmget(ins_m_id_redis, { "user_id", "msg_text", "msg_time" }, back_inserter(fields));

			for (const auto& val : fields) {
				if (val) {
					std::cout << *val << " / ";
					redis_to_mysql[index_f] = *val;			// redis 데이터 배열로 받아옴

					index_f++;
				}
				else {				// Redis에 데이터 없으면 탈출
					break;
				}
			}
			cout << endl;

			outp_msg_id += 1;				// redis에서 빼와야할 채팅 갯수 // 다 빼오면 +1
		}
		catch (const Error& e) {
			cerr << "Redis 연결 오류: " << e.what() << endl;
		}
	}

public:
	Chat_send(int _user_id, const char* _msg_text, const char* _msg_time, sql::Connection* _m_conn, shared_ptr<Redis> _redis)
		: user_id(_user_id), msg_text(_msg_text), msg_time(_msg_time), m_conn(_m_conn), redis(_redis) {
		stat_check();		// 객체 생성 시 유저 상태 확인
	}

	void insert_chat(string _msg_text) {		// 채팅 저장 함수
		stat_check();		// 차단 됐는지 확인 후 값이 변경 됐으면 변경
		if (user_status != 3) {			// 임시 차단이 아니면 아래 코드 실행
			cout << "채팅 입력 ";
			getline(cin, msg_text);		// 채팅 입력 받기
			current_date();		// 채팅 입력 후 바로 시간 입력받기

			insert_chat_redis();		// 입력받은 채팅 redis로 저장
		}
		else {
			cout << "차단 상태" << endl;
		}
	}

	void insert_chat_redis() {		// 입력받은 채팅 redis로 저장하는 함수
		try {
			local_msg_id = room_msg_num++;				// 현재 채팅 내역을 local_msg_id에 저장하면서 전역 변수 값 +1 증가
			string ins_m_id_redis = "msg_id:" + to_string(local_msg_id);		// msg_id 동적으로 입력받기
			string r_msg_id = to_string(user_id);

			unordered_map<string, string> cols = { {"user_id", r_msg_id}, {"msg_text", msg_text}, {"msg_time", msg_time} };
			redis->hmset(ins_m_id_redis, cols.begin(), cols.end());
		}
		catch (const Error& e) {
			cerr << "Redis 연결 오류: " << e.what() << endl;
		}
	}

	void insert_chat_mysql() {			// Redis에 담은 채팅 데이터 MySQL로 이동
		r_all_msg = room_msg_num;			// redis에 저장해야할 채팅 갯수


		while (true) {
			out_chat_data_redis();				// redis 데이터 담기

			if (!redis_to_mysql[0].empty()) {		// redis 에서 데이터를 성공적으로 담은지 확인

				del_chat_redis();					// 성공적으로 redis에서 클라이언트로 데이터를 담으면 Redis 데이터 삭제

				int _user_id = stoi(redis_to_mysql[0]);

				try {
					unique_ptr<PreparedStatement> pstmt(m_conn->prepareStatement("INSERT INTO Message (user_id, msg_text, msg_time) VALUES(?, ?, ?)"));
					pstmt->setInt(1, _user_id);
					pstmt->setString(2, redis_to_mysql[1]);
					pstmt->setDateTime(3, redis_to_mysql[2]);
					pstmt->executeUpdate();
				}
				catch (SQLException& e) {		// 쿼리 연결 오류
					cout << "MySQL Connection Failed: " << e.what() << endl;
					cout << "SQL Error Code: " << e.getErrorCode() << endl;
					cout << "SQL State: " << e.getSQLState() << endl;
				}
				redis_to_mysql[0] = "";				// 다시 redis에서 꺼낸 값 초기화
			}
			else {
				cout << "클라이언트에 Redis 데이터 없음!" << endl;
				break;
			}
		}

	}
};