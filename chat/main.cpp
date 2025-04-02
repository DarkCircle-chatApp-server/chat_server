#include "DB.hpp"
#include <iostream>
#include "httplib.h"
#include "chat_send.hpp"
#include "chat_room.hpp"
#include "chat_print.hpp"
#include <windows.h>

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

    MySQLConnector db(MYSQL_SERVER_IP, MYSQL_USERNAME, MYSQL_PASSWORD, MYSQL_DATABASE);
    sql::Connection* s_conn = mysql_db_conn();              // MySQL DB연동


    R_Conn r_conn;
    auto redis = make_shared<Redis>(r_conn.opts);

    Chat_room user(redis);
    Chat_send client(1, "", "", s_conn, redis);

    // 채팅방 입장(실행)
    thread room(&Chat_room::ch_room, &user);
    room.detach();

    // 채팅 입력 및 채팅 값 바로 redis에 저장
    svr.Post("/chat/room", [&](const httplib::Request& req, httplib::Response& res) {
        user.ch_talk(req, res);
        client.insert_chat(req, res);
        });

    // redis에 저장된 데이터 mysql에 저장
    svr.Post("/chat/room/mysql", [&](const httplib::Request& req, httplib::Response& res) {
        client.insert_chat_mysql(req, res);
        });

    Message select(db.getConnection());  // GET 요청 처리
    svr.Get("/chat/messages", [&](const httplib::Request& req, httplib::Response& res) {
        select.handleMessages(req, res);
        });

    //svr.Get("/chat", handleChat);

    //svr.Post("/chat", [&](const httplib::Request& req, httplib::Response& res) {        // json 요청받기 위해 chat_insert()함수 연동
    //    test.insert_chat(req, res);
    //    });

    // CORS 설정
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << u8"Chat Service running: http://localhost:5003" << std::endl;
    svr.listen("0.0.0.0", 5003); // 서버 실행

    // return 0; 하면 안 됨, 서버는 종료될 때까지 계속 실행되어야 함
}