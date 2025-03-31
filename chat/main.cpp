#include <iostream>
#include "httplib.h"
#include "chat_send.hpp"
#include "DB.hpp"
#include "test.hpp"

using namespace sql;
using json = nlohmann::json;
// 채팅 관련 함수
void handleChat(const httplib::Request& req, httplib::Response& res) {

    // 내부 로직 기능

    res.set_content("chat", "text/plain");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);

    httplib::Server svr;    // httplib::Server 객체 생성

    // 메인함수에서 임시로 돌려본 코드
    Connection* conn = mysql_db_conn();

    Chat_send test(1, "", "", conn);

    Message select(db.getConnection());  // GET 요청 처리
    svr.Get("/chat/messages", [&](const httplib::Request& req, httplib::Response& res) {
        select.handleMessages(req, res);
        });

    //svr.Get("/chat", handleChat);

    svr.Post("/chat", [&](const httplib::Request& req, httplib::Response& res) {        // json 요청받기 위해 chat_insert()함수 연동
        test.insert_chat(req, res);
        });

    // CORS 설정
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << "Chat Service 실행 중: http://localhost:5003" << std::endl;
    svr.listen("0.0.0.0", 5003); // 서버 실행

    // return 0; 하면 안 됨, 서버는 종료될 때까지 계속 실행되어야 함
}