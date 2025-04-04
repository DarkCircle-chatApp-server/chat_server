#include <sw/redis++/redis++.h>
#include "DB.hpp"
#include <iostream>
#include "httplib.h"
#include "chat_send.hpp"
#include "chat_room.hpp"
#include "chat_print.hpp"
#include <windows.h>
#include <vector>


using namespace std;
using namespace sql;
using json = nlohmann::json;


// 채팅 관련 함수 
void handleChat(const httplib::Request& req, httplib::Response& res) {

    // 내부 로직 기능

    res.set_content("chat", "text/plain");
}

unique_ptr<sql::Connection> s_conn = mysql_db_conn();              // MySQL DB연동
R_Conn r_conn;
auto redis = make_unique<Redis>(r_conn.opts);
Chat_room rooms(move(redis));          // 채팅방 생성 및 입장 클래스

unordered_map<int, unique_ptr<Chat_send>> user_set;
mutex user_set_m;

void user_login_and_chat(int id, std::string body_json) {
    {
        try {
            std::lock_guard<std::mutex> lock(user_set_m);
            user_set[id] = std::make_unique<Chat_send>(
                id, "", "",
                mysql_db_conn(),
                make_unique<Redis>(r_conn.opts)
            );
            std::cout << id << " 접속 완료! (스레드)" << std::endl;
        }
        catch (const exception& e) {
            cerr << "Chat_send 생성 중 예외" << e.what() << endl;
        }
    }
    httplib::Request dummy_req;
    dummy_req.body = body_json;

    httplib::Response dummy_res;

    {
        std::lock_guard<std::mutex> lock(user_set_m);
        user_set[id]->insert_chat(dummy_req, dummy_res);
    }
}

int main() {
    //SetConsoleOutputCP(CP_UTF8);
    httplib::Server svr;    // httplib::Server 객체 생성

    //MySQLConnector db(MYSQL_SERVER_IP, MYSQL_USERNAME, MYSQL_PASSWORD, MYSQL_DATABASE);

    //Chat_send client(1, "", "", s_conn, redis);

    //unordered_map<int, unique_ptr<Chat_send>> user_set;
    //user_set[id] = make_unique<Chat_send>(id, "", "", s_conn, redis);

    //return 0;       // 소멸자 확인용

    svr.Options("/chat/room", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/sse", [](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    // 채팅 입력 및 채팅 값 바로 redis에 저장
    svr.Post("/chat/room", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        //res.set_header("Access-Control-Allow-Origin", "*");
        std::cout << "/chat/room 요청 받음" << std::endl;
        std::cout << u8"요청 내용: " << req.body << std::endl;
        json req_json = json::parse(req.body);
        int user_id = req_json["user_id"];
        
        rooms.ch_talk(req, res);
        lock_guard<mutex> lock(user_set_m);
        if (user_set.find(user_id) != user_set.end()) {
            user_set[user_id]->insert_chat(req, res);
        }
        else {
            std::string req_body = req.body;
            std::thread t(user_login_and_chat, user_id, req_body);
            t.detach();

        }
        //client.insert_chat(req, res);
        });


    Message select(move(s_conn));  // GET 요청 처리
    svr.Get("/chat/messages", [&](const httplib::Request& req, httplib::Response& res) {
        select.handleMessages(req, res);
        });


    // subsriber 서버 <-> sse
    svr.Get("/chat/sse", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "text/event-stream");
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");
        std::cout << "subscribe channel opened" << endl;
        rooms.sse_handler(req, res);
        });

    //svr.Get("/chat", handleChat);

    //svr.Post("/chat", [&](const httplib::Request& req, httplib::Response& res) {        // json 요청받기 위해 chat_insert()함수 연동
    //    test.insert_chat(req, res);
    //    });

    // CORS 설정
    //svr.set_default_headers({
    //    { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
    //    { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
    //    { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
    //    });


    std::cout << u8"Chat Service 실행 중: http://localhost:8881" << std::endl;
    svr.listen("0.0.0.0", 8881); // 서버 실행


    // return 0; 하면 안 됨, 서버는 종료될 때까지 계속 실행되어야 함
}