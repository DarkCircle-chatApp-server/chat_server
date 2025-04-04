#pragma once

<<<<<<< Updated upstream
=======
#include <sw/redis++/redis++.h>
//#include <nlohmann/json.hpp>
>>>>>>> Stashed changes
#include "json.hpp"
#include "httplib.h"
#include "DB.hpp"
#include "chat_send.hpp"

using json = nlohmann::json;

class Chat_room {
private:
	string msg_text = "";			// 채팅 메시지
	
	shared_ptr<Redis> redis;
public:
	Chat_room(shared_ptr<Redis> _redis) : redis(_redis) {

	}

	void ch_room() {				// 채팅 방 함수
		Subscriber sub = ch_recive();			// 유저 입장(Thread 상태)
		chrono::milliseconds(1000);				// 방 입장 전 잠시 대기

		while (1) {
			try {
				sub.consume();
			}
			catch (const Error& e) {
				cerr << "sub error : " << e.what() << endl;
			}
		}
	}

	void ch_talk(const httplib::Request& req, httplib::Response& res) {				// 채팅 입력 함수
		//cout << "채팅 입력 : ";
		//getline(cin, msg_text);
		json req_json = json::parse(req.body);
		msg_text = req_json["msg_text"];
		redis->publish("chat_room:1", msg_text);		// 채팅방에 보내기

	}

	Subscriber ch_recive() {						// 채팅방 입장 함수
		auto sub = redis->subscriber();
		sub.subscribe("chat_room:1");				// 채팅방 이름
		sub.on_message([](string channel, string msg) {			// 전송받을 메시지 타입 + 포맷
			cout << "Chat_room:1 : " << msg << endl;
			});
		return sub;
	}
};