#pragma once

#include "DB.hpp"
#include "chat_send.hpp"

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

		while (1) {							// 채팅 입력 (exit 방 나가기)
			if (msg_text == "exit") break;
			try {
				sub.consume();
			}
			catch (const Error& e) {
				cerr << "sub error : " << e.what() << endl;
			}
			ch_talk();
		}
	}

	void ch_talk() {				// 채팅 입력 함수
		cout << "채팅 입력 : ";
		getline(cin, msg_text);
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