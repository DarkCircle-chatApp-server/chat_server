#pragma once

#include <sw/redis++/redis++.h>
//#include <nlohmann/json.hpp>
#include "json.hpp"
#include "httplib.h"
#include "DB.hpp"
#include "chat_send.hpp"
#include <mutex>
#include <atomic>
#include <vector>
#include <queue>


using json = nlohmann::json;

static std::vector<json> participants;
std::mutex participants_mutex;

class Chat_room {
private:
	string msg_text = "";			// 채팅 메시지

	unique_ptr<Redis> redis;

	void ch_room() {		// 채팅 방 입장 함수	/	외부에서 호출 X
		//Subscriber sub = ch_set();			// 유저 입장(Thread 상태)
		//cout << "접속 대기 " << endl;
		this_thread::sleep_for(chrono::milliseconds(500));			// 방 입장 전 잠시 대기
		cout << u8"chat_room:1 접속 완료" << endl;

		/*while (1) {
			try {
				sub.consume();
			}
			catch (const Error& e) {
				cerr << "sub error : " << e.what() << endl;
			}
		}*/
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
	Chat_room(unique_ptr<Redis> _redis) : redis(move(_redis)) {
		thread room([&]() {		// Chat_room 클래스 생성 시 시작(채팅방 입장)
			ch_room();
			});
		room.detach();		// ch_room 스레드 분리
	}

	void ch_talk(const httplib::Request& req, httplib::Response& res) {				// 채팅 입력 함수
		//cout << "채팅 입력 : ";
		//getline(cin, msg_text);
		json req_json = json::parse(req.body);
		int user_id = req_json["user_id"];
		int user_status = req_json["user_status"];
		msg_text = req_json["msg_text"];
		string user_name = req_json["user_name"];
		json message_json = {
			{"user_id", user_id},
			{"msg_text", msg_text},
			{"user_status", user_status},
			{"user_name", user_name}
		};
		if (user_status != 3) {
			redis->publish("chat_room:1", message_json.dump());		// 채팅방에 보내기
		}
		//redis->publish("chat_room:1", msg_text);
		std::cout << "Published message: " << msg_text << std::endl;
		res.set_content(R"({"status": "ok", "message": "Message sent"})", "application/json");
	}

	// 접속 시 참여자 데이터 redis로 전송
	void on_user_join(const httplib::Request& req, httplib::Response& res) {
		std::cout << "redis 포인터 상태: " << (redis ? "OK" : "nullptr") << std::endl;
		json req_json = json::parse(req.body);
		int user_id = req_json["user_id"];
		string user_name = req_json["user_name"];
		json user_info = {
			{"user_id", user_id},
			{"user_name", user_name}
		};
		//static std::vector<json> participants;
		std::lock_guard<std::mutex> lock(participants_mutex);
		participants.push_back(user_info);

		json broadcast_data = {
			{"type", "participants"},
			{"participants", participants}
		};

		redis->publish("chat_room:1", broadcast_data.dump());
		std::cout << "Entered user: " << user_name << std::endl;
		res.set_content(R"({"status": "ok", "user": "Entered"})", "application/json");
	}

	// 채팅방 퇴장
	void on_user_exit(const httplib::Request& req, httplib::Response& res) {
		json req_json = json::parse(req.body);
		int user_id = req_json["user_id"];

		std::lock_guard<std::mutex> lock(participants_mutex);

		std::cout << "요청으로 받은 user_id: " << user_id << std::endl;
		std::cout << "현재 participants 목록: " << std::endl;
		for (const auto& p : participants) {
			std::cout << p.dump() << std::endl;
		}

		// user_id가 일치하는 사람만 제거
		for (auto it = participants.begin(); it != participants.end(); ++it) {
			if ((*it)["user_id"] == user_id) {
				participants.erase(it);
				break;
			}
		}

		// 남은 참가자 목록을 브로드캐스트
		json broadcast_data = {
			{"type", "participants"},
			{"participants", participants}
		};
		redis->publish("chat_room:1", broadcast_data.dump());

		res.set_content(R"({"status": "ok", "message": "user exited"})", "application/json");
	}

	// react sse 통신 연동
	void sse_handler(const httplib::Request&, httplib::Response& res) {
		res.set_header("Content-Type", "text/event-stream");
		res.set_header("Cache-Control", "no-cache");
		res.set_header("Connection", "keep-alive");

		// 초기 데이터
		std::string initial_data;
		{
			std::lock_guard<std::mutex> lock(participants_mutex);
			json init_payload = {
				{"type", "participants"},
				{"participants", participants}
			};
			initial_data = "data: " + init_payload.dump() + "\n\n";
		}

		auto sub = std::make_shared<sw::redis::Subscriber>(redis->subscriber());
		sub->subscribe("chat_room:1");

		// 메시지 담아둘 큐 (스레드 간 안전하게 공유)
		auto msg_queue = std::make_shared<std::queue<std::string>>();
		auto queue_mutex = std::make_shared<std::mutex>();

		// Redis 수신 쓰레드: 메시지를 큐에 push
		std::thread([sub, msg_queue, queue_mutex]() {
			try {
				sub->on_message([msg_queue, queue_mutex](std::string, std::string msg) {
					std::lock_guard<std::mutex> lock(*queue_mutex);
					msg_queue->push("data: " + msg + "\n\n");
					});

				while (true) {
					sub->consume();
				}
			}
			catch (const std::exception& e) {
				std::cerr << "Redis Subscribe Error: " << e.what() << std::endl;
			}
			}).detach();

		res.set_chunked_content_provider("text/event-stream",
			[initial_data, msg_queue, queue_mutex](size_t, httplib::DataSink& sink) mutable -> bool {
				try {
					// 최초 참여자 정보 전송
					sink.write(initial_data.data(), initial_data.size());
				}
				catch (...) {
					return false;
				}

				// 이후 메시지 큐에서 꺼내서 전송
				while (true) {
					std::string next_msg;

					{
						std::lock_guard<std::mutex> lock(*queue_mutex);
						if (!msg_queue->empty()) {
							next_msg = msg_queue->front();
							msg_queue->pop();
						}
					}

					if (!next_msg.empty()) {
						try {
							sink.write(next_msg.data(), next_msg.size());
						}
						catch (...) {
							return false;
						}
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 과도한 루프 방지
				}

				return true;
			}
		);
	}


};