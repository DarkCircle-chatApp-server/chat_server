#pragma once

#include "json.hpp"
#include "httplib.h"
#include "DB.hpp"
#include "chat_send.hpp"

using json = nlohmann::json;

class Chat_room {
private:
	string msg_text = "";			// 채팅 메시지
	
	shared_ptr<Redis> redis;

	void ch_room() {		// 채팅 방 입장 함수	/	외부에서 호출 X
		Subscriber sub = ch_set();			// 유저 입장(Thread 상태)
		//cout << "접속 대기 " << endl;
		this_thread::sleep_for(chrono::milliseconds(500));			// 방 입장 전 잠시 대기
		cout << "접속 완료" << endl;

		while (1) {
			try {
				sub.consume();
			}
			catch (const Error& e) {
				cerr << "sub error : " << e.what() << endl;
			}
		}
	}

	Subscriber ch_set() {		// 채팅방 세팅 함수		/	외부에서 호출 X
		auto sub = redis->subscriber();
		sub.subscribe("chat_room:1");				// 채팅방 이름
		sub.on_message([](string channel, string msg) {			// 전송받을 메시지 타입 + 포맷
			cout << "Chat_room:1 : " << msg << endl;
			});
		return sub;
	}

public:
	Chat_room(shared_ptr<Redis> _redis) : redis(_redis) {
		thread room([&]() {		// Chat_room 클래스 생성 시 시작(채팅방 입장)
			ch_room();
			});
		room.detach();		// ch_room 스레드 분리
	}

	void ch_talk(const httplib::Request& req, httplib::Response& res) {				// 채팅 입력 함수
		//cout << "채팅 입력 : ";
		//getline(cin, msg_text);
		json req_json = json::parse(req.body);
		msg_text = req_json["msg_text"];
		redis->publish("chat_room:1", msg_text);		// 채팅방에 보내기

	}
};