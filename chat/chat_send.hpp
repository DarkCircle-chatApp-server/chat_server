#pragma once

#include "json.hpp"
#include "httplib.h"
#include "DB.hpp"
#include <ctime>

using json = nlohmann::json;

static int room_msg_num = 1;			// 채팅방 채팅 번호(갯수)

class Chat_send {
protected:
	const int user_id = 0;				// 유저 id 상수화 선언 (객체 생성시 고정 아이디 값)
	int local_msg_id = 1;				// redis에 저장하기 직전 입력받은 채팅의 번호

	int user_status = 0;				// 유저 상태값 상수화 선언 (디버그 모드일때 오류나서 일단 상수X)
	
	int r_del_num = 1;					// Redis에서 지워야할 msg_id 첫번째 값을 저장하는 변수
	int r_data_num = 1;					// Redis에서 sql로 데이터를 넣은 후 Redis에 지워야 할 데이터 수
	int check_s_data = 1;				// 처음 s_data_num 값
	int s_data_num = 1;					// MySQL로 넘겨줄 데이터 수

	string msg_text;		// 메시지 내용
	string msg_time;		// 타임 로그

	string redis_to_mysql[3] = {};			// redis에 저장된 데이터 sql로 옮기는 배열

	shared_ptr<sql::Connection> m_conn;			// SQL 의존성 주입 객체
	shared_ptr<Redis> redis;			// Redis 의존성 주입 객체


	recursive_mutex del_redis;

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

	bool del_chat_redis() {				// redis 데이터 삭제 함수
		if (r_del_num == 1 && s_data_num == 1) return false;		// 첫 실행 시 스킵

		for (int i = r_del_num; i < s_data_num; i++) {
			string ins_m_id_redis = "msg_id:" + to_string(i);
			try {
				redis->del(ins_m_id_redis);
			}
			catch (const Error& e) {
				cerr << "Redis 연결 오류: " << e.what() << endl;
				r_data_num = i;			// 지금까지 삭제한 값 넣기
				return false;
			}
		}
		return true;
	}

	void out_chat_data_redis() {			// redis 데이터 불러오기
		try {
			int index_f = 0;				// redis_to_mysql 배열에 동적으로 넣기 위한 인덱스

			string ins_m_id_redis = "msg_id:" + to_string(s_data_num);		// msg_id 동적으로 입력받기

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
		}
		catch (const Error& e) {
			cerr << "Redis 연결 오류: " << e.what() << endl;
		}
	}

public:
	Chat_send(int _user_id, const char* _msg_text, const char* _msg_time, shared_ptr<sql::Connection> _m_conn, shared_ptr<Redis> _redis)
		: user_id(_user_id), msg_text(_msg_text), msg_time(_msg_time), m_conn(move(_m_conn)), redis(_redis) {
		stat_check();		// 객체 생성 시 유저 상태 확인

		if (user_id == 1) {			// 계정이 관리자 일 때 실행	(관리자 id db에 저장 안되있어서 1로 함 나중에 수정)
			thread auto_save([&]() {		// auto save 스레드 생성
				auto_save_mysql();
				});
			auto_save.detach();			// auto_save_mysql 함수 스레드 분리
		}
	}
	~Chat_send() {
		if (user_id == 1) insert_chat_mysql();			// 관리자가 프로그램을 정상 종료해야 저장됨 (관리자 id db에 저장안되있어서 일부러 1로함)
	}

	void insert_chat(const httplib::Request& req, httplib::Response& res) {
		stat_check();		// 차단 됐는지 확인 후 값이 변경 됐으면 변경
		json req_json = json::parse(req.body);

		if (user_status != 3) {			// 임시 차단이 아니면 아래 코드 실행
			//cout << "채팅 입력 ";
			//getline(cin, msg_text);		// 채팅 입력 받기
			msg_text = req_json["msg_text"];
			current_date();		// 채팅 입력 후 바로 시간 입력받기

			insert_chat_redis();		// 입력받은 채팅 redis로 저장
			std::cout << "Chat Inserted into Redis: " << msg_text << std::endl;
			//res.set_content(R"({"status": "ok", "message": "Chat saved"})", "application/json");
		}
		else {
			cout << "차단 상태" << endl;
			res.set_content(R"({"status": "blocked", "message": "User is blocked"})", "application/json");
		}
	}

	void insert_chat_redis() {		// 입력받은 채팅 redis로 저장하는 함수
		try {
			local_msg_id = room_msg_num;	// 현재 채팅 내역을 local_msg_id에 저장하면서 전역 변수 값 +1

			string ins_m_id_redis = "msg_id:" + to_string(local_msg_id);		// msg_id 동적으로 입력받기
			string r_msg_id = to_string(user_id);

			unordered_map<string, string> cols = { {"user_id", r_msg_id}, {"msg_text", msg_text}, {"msg_time", msg_time} };
			redis->hmset(ins_m_id_redis, cols.begin(), cols.end());

			room_msg_num++;					// 성공하면 +1
		}
		catch (const Error& e) {
			cerr << "Redis 연결 오류: " << e.what() << endl;
		}
	}

	void auto_save_mysql() {			// 자동 저장 함수
		while (true) {
			insert_chat_mysql();
			this_thread::sleep_for(chrono::minutes(1));			// 자동 저장 시간 설정(분)
		}
	}

	void insert_chat_mysql() {			// Redis에 담은 채팅 데이터 MySQL로 이동
		//json req_json = json::parse(req.body);
		
		if (r_data_num > s_data_num) {
			del_chat_redis();
		}

		m_conn->setAutoCommit(false);		// 트랜잭션 적용

		r_del_num = r_data_num;					// Redis에서 지워야할 msg_id 첫번째 값 저장
		r_data_num = room_msg_num;				// Redis에서 MySQL로 데이터를 넘겨야 할 갯수
		check_s_data = s_data_num;				// 쿼리문 돌기 전 sql 초기값

		while (true) {
			out_chat_data_redis();				// redis 데이터 담기

			if (!redis_to_mysql[0].empty()) {		// redis 에서 데이터를 성공적으로 담은지 확인

				int change_id = stoi(redis_to_mysql[0]);			// redis에 저장된 user_id 값 형 변환

				try {
					unique_ptr<PreparedStatement> pstmt(m_conn->prepareStatement("INSERT INTO Message (user_id, msg_text, msg_time) VALUES(?, ?, ?)"));
					pstmt->setInt(1, change_id);
					pstmt->setString(2, redis_to_mysql[1]);
					pstmt->setDateTime(3, redis_to_mysql[2]);
					pstmt->executeUpdate();

					s_data_num++;				// 남은 메시지 1증가
				}
				catch (SQLException& e) {		// 쿼리 연결 오류
					cout << "MySQL Connection Failed: " << e.what() << endl;
					cout << "SQL Error Code: " << e.getErrorCode() << endl;
					cout << "SQL State: " << e.getSQLState() << endl;

					s_data_num = check_s_data;	// 데이터 저장 전으로 돌아감
					redis_to_mysql[0] = "";		// redis에서 꺼낸 값 초기화
					m_conn->rollback();			// 트랜잭션 롤백

					return;				// 쿼리 실패 = 함수 탈출
				}

				//res.set_content("user_id: " + redis_to_mysql[0] + " 추출 중..", "text/plain");		// json 데이터로 웹에 실행 완료 메시지 보냄
				redis_to_mysql[0] = "";				// 다시 redis에서 꺼낸 값 초기화
			}
			else {				// Redis에서 MySQL로 성공적으로 데이터 다 담으면 탈출
				cout << "클라이언트에 Redis 데이터 없음!" << endl;
				break;
			}
		}
		m_conn->commit();				// 트랜잭션 완료
		//res.set_content("Success Redis into MySQL", "text/plain");			// json 데이터로 웹에 실행 완료 메시지 보냄

		del_chat_redis();			// mysql 데이터 성공적으로 넣으면 redis 데이터 삭제
	}
};