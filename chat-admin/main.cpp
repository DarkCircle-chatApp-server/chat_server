#include "httplib.h"
#include <iostream>
#include"DB_admin.hpp"
#include "chat_ban.hpp"
#include "user_check.hpp"

// 채팅 관리자 기능 함수
//void handle_chat_admin(const httplib::Request& req, httplib::Response& res) {
//
//    res.set_content("chat admin", "text/plain"); // 성공 응답
//}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    
    httplib::Server svr;    // httplib::Server 객체 생성
    User_check User_check(db.getConnection());
    // 회원 목록 조회
    svr.Get("/chat/admin/check", [&](const httplib::Request& req, httplib::Response& res) {
        User_check.handle_user_check(req, res);
    });

    //User_ban user_ban(db.getConnection());
    //// 밴 처리
    //svr.Put("/chat/admin/ban", [&](const httplib::Request& req, httplib::Response& res) {
    //    user_ban.handle_user_ban(req, res);
    //    });

    //// 밴해제 처리
    //User_ban user_unban(db.getConnection());

    //svr.Put("/chat/admin/unban", [&](const httplib::Request& req, httplib::Response& res) {
    //    user_unban.handle_user_unban(req, res);
    //    });

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
