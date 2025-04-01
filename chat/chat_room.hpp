#pragma once

#include "DB.hpp"

class Chat_room {
private:
	string msg_text = "";
	shared_ptr<Redis> redis;
public:
	Chat_room(shared_ptr<Redis> _redis) : redis(_redis) {

	}

	void ch_room() {
		Subscriber sub = ch_recive();
		chrono::milliseconds(1000);

		while (1) {
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

	void ch_talk() {
		cout << "채팅 입력 : ";
		getline(cin, msg_text);
		redis->publish("chat_room:1", msg_text);
	}

	Subscriber ch_recive() {
		auto sub = redis->subscriber();
		sub.subscribe("chat_room:1");
		sub.on_message([](string channel, string msg) {
			cout << "Chat_room:1 : " << msg << endl;
			});
		return sub;
	}
};