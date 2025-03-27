#include "httplib.h"
#include <iostream>
#include"DB_admin.hpp"
#include "chat_ban.hpp"

// 채팅 관리자 기능 함수
void handle_chat_admin(const httplib::Request& req, httplib::Response& res) {

    res.set_content("chat admin", "text/plain"); // 성공 응답
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    //ban
    User_ban user_ban(db.getConnection());
    user_ban.set_user_id("1");
    user_ban.User_ban_func();
    //unban
    user_ban.User_unban_func();

    httplib::Server svr;    // httplib::Server 객체 생성

    svr.Get("/chat/admin", handle_chat_admin);

    // CORS 설정
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << "Chat Service running: http://localhost:5004" << std::endl;
    svr.listen("0.0.0.0", 5004); // 서버 실행
    

    // return 0; 하면 안 됨, 서버는 종료될 때까지 계속 실행되어야 함
}
