#include "DB.hpp"

#include <iostream>
#include "httplib.h"
#include "chat_send.hpp"
#include "chat_room.hpp"
#include "chat_print.hpp"

using namespace sql;
using json = nlohmann::json;
// 채팅 관련 함수 
void handleChat(const httplib::Request& req, httplib::Response& res) {

    // 내부 로직 기능

    res.set_content("chat", "text/plain");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    httplib::Server svr;    // httplib::Server 객체 생성

    MySQLConnector db(MYSQL_SERVER_IP, MYSQL_USERNAME, MYSQL_PASSWORD, MYSQL_DATABASE);
    sql::Connection* s_conn = mysql_db_conn();              // MySQL DB연동

    //R_Conn r_conn;
    ////auto redis = make_shared<Redis>(r_conn.opts);

    //Chat_send ch_send(1, "", "", s_conn, redis);
    //Chat_room ch_room(redis);

    //ch_send.insert_chat();                  // 채팅 입력 후 redis로 전송
    //ch_send.insert_chat_mysql();            // redis에 저장된 채팅 데이터 mysql로 전송
    
    //ch_room.ch_room();

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

    std::cout << u8"Chat Service 실행 중: http://localhost:5003" << std::endl;
    svr.listen("0.0.0.0", 5003); // 서버 실행

    // return 0; 하면 안 됨, 서버는 종료될 때까지 계속 실행되어야 함
}