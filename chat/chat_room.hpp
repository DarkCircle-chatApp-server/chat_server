#pragma once
#include <sw/redis++/redis++.h>
#include <nlohmann/json.hpp>
#include "httplib.h"
#include "DB.hpp"
#include "chat_send.hpp"
#include <mutex>
#include <atomic>

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
		cout << "enter chat_room:1" << endl;

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
		//int user_id = req_json["user_id"];
		msg_text = req_json["msg_text"];
		redis->publish("chat_room:1", msg_text);		// 채팅방에 보내기
		cout << "published message" << endl;
		//res.set_content(R"({"status": "ok", "message": "Message sent"})", "application/json");
	}

	Subscriber ch_recive() {						// 채팅방 입장 함수
		auto sub = redis->subscriber();
		sub.subscribe("chat_room:1");				// 채팅방 이름
		sub.on_message([](string channel, string msg) {			// 전송받을 메시지 타입 + 포맷
			cout << "Chat_room:1 : " << msg << endl;
			});
		return sub;
	}


	// react sse 통신 연동
	void sse_handler(const httplib::Request&, httplib::Response& res) {
		res.set_header("Content-Type", "text/event-stream");
		res.set_header("Cache-Control", "no-cache");
		res.set_header("Connection", "keep-alive");

		auto sub = std::make_shared<sw::redis::Subscriber>(redis->subscriber());
		sub->subscribe("chat_room:1");

		res.set_chunked_content_provider("text/event-stream",
			[sub](size_t, httplib::DataSink& sink) mutable -> bool {
				std::thread([sub, &sink]() mutable {
					try {
						sub->on_message([&sink](std::string channel, std::string msg) {
							std::cout << "Redis 메시지 수신됨: " << msg << std::endl;
							std::string data = "data: " + msg + "\n\n";
							sink.write(data.data(), data.size());
							});

						while (true) {
							sub->consume();
						}
					}
					catch (const std::exception& e) {
						std::cerr << "Redis Subscribe Error: " << e.what() << std::endl;
					}
					}).detach(); // 백그라운드 스레드 실행

				return true;  // 스트리밍 유지
			}
		);
	}
};