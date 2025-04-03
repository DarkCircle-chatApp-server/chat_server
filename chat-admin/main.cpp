#include "httplib.h"
#include <iostream>
#include"DB_admin.hpp"
#include "chat_ban.hpp"
#include "admin_select_delete.hpp"
#include "change_pw.hpp"

// 채팅 관리자 기능 함수
//void handle_chat_admin(const httplib::Request& req, httplib::Response& res) {
//
//    res.set_content("chat admin", "text/plain"); // 성공 응답
//}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    
    httplib::Server svr;                                                                    // httplib::Server 객체 생성

    

    Select_delete admin_select(db.getConnection());                                               // 회원 조회 GET 요청 처리
    svr.Get("/chat/admin/admin_select", [&](const httplib::Request& req, httplib::Response& res) {
        admin_select.handle_admin_select(req, res);
        });

    Select_delete user_select(db.getConnection());                                               // 개인 조회 POST 요청 처리
                                                                                                 // 입력받을 body가 있어야되서 post 사용함
    svr.Post("/chat/admin/user_select", [&](const httplib::Request& req, httplib::Response& res) {
		cout << "user_select" << endl;
        user_select.handle_user_select(req, res);
        });


    Select_delete user_delete(db.getConnection());                                          // 유저 삭제
    svr.Put("/chat/admin/user_delete", [&](const httplib::Request& req, httplib::Response& res) {
        user_delete.handle_admin_user_delete(req, res);
        });

    Select_delete message_delete(db.getConnection());                                       // 메세지 삭제
    svr.Put("/chat/admin/message_delete", [&](const httplib::Request& req, httplib::Response& res) {
        message_delete.handle_amdim_message_delete(req, res);
        });

    Select_delete admin_status(db.getConnection());                                                // 관리자 권한 부여
    svr.Put("/chat/admin/status_update", [&](const httplib::Request& req, httplib::Response& res) {
        admin_status.handle_admin_status(req, res);
        });

    User_ban user_ban(db.getConnection());                                                  // 밴 처리
    svr.Put("/chat/admin/ban", [&](const httplib::Request& req, httplib::Response& res) {
        user_ban.handle_user_ban(req, res);
        });

    
    User_ban user_unban(db.getConnection());                                                // 밴해제 처리
    svr.Put("/chat/admin/unban", [&](const httplib::Request& req, httplib::Response& res) {
        user_unban.handle_user_unban(req, res);
        });


    Change_PW change_pw(db.getConnection());                                                // 비밀번호 변경
    svr.Put("/chat/admin/change_pw", [&](const httplib::Request& req, httplib::Response& res) {
        change_pw.handle_Change_PW(req, res);
        });


    // CORS 설정
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },                                             // 모든 도메인에서 접근 허용
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << "Chat Service running: http://localhost:8882" << std::endl;
    svr.listen("0.0.0.0", 8882); // 서버 실행
    

    // return 0; 하면 안 됨, 서버는 종료될 때까지 계속 실행되어야 함
}
