#include <sw/redis++/redis++.h>
#include "DB.hpp"
#include <iostream>
#include "httplib.h"
#include "chat_send.hpp"
#include "chat_room.hpp"
#include "chat_print.hpp"
#include <windows.h>

using namespace std;
using namespace sql;
using json = nlohmann::json;
// 채팅 관련 함수 
void handleChat(const httplib::Request& req, httplib::Response& res) {

    // 내부 로직 기능

    res.set_content("chat", "text/plain");
}

int main() {
    //SetConsoleOutputCP(CP_UTF8);
    httplib::Server svr;    // httplib::Server 객체 생성



    //MySQLConnector db(MYSQL_SERVER_IP, MYSQL_USERNAME, MYSQL_PASSWORD, MYSQL_DATABASE);
    shared_ptr<sql::Connection> s_conn = mysql_db_conn();              // MySQL DB연동

    R_Conn r_conn;
    auto redis = make_shared<Redis>(r_conn.opts);

    Chat_room user(redis);          // 채팅방 생성 및 입장 클래스
    Chat_send client(1, "", "", s_conn, redis);

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
        user.ch_talk(req, res);
        client.insert_chat(req, res);
        });

    // redis에 저장된 데이터 mysql에 저장
    svr.Post("/chat/room/mysql", [&](const httplib::Request& req, httplib::Response& res) {

        cout << "insert_chat_mysql" << endl;
        client.insert_chat_mysql();

        });

    Message select(s_conn);  // GET 요청 처리
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
        user.sse_handler(req, res);
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