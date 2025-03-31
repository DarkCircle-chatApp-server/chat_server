#include "httplib.h"
#include <iostream>
#include"DB_admin.hpp"
#include "chat_ban.hpp"


#include "admin_select_delete.hpp"

// 채팅 관리자 기능 함수
//void handle_chat_admin(const httplib::Request& req, httplib::Response& res) {
//
//    res.set_content("chat admin", "text/plain"); // 성공 응답
//}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    
    httplib::Server svr;    // httplib::Server 객체 생성
    Select_delete select_delete(db.getConnection());  // getConnection()에서 반환된 MySQLConnector의 conn을 signin객체에 주입
    select_delete.All_Select();  // user 테이블 조회
    string user_id;
    cout << u8"회원 삭제할 user_id: ";
    cin >> user_id;
    select_delete.Update_Status(user_id); //  user_id의 user_status를 2로 변경
    //select_delete.Update_Status2(user_id); //  2초후 자동으로 원복 실험 -> 유저가 회원탈퇴시 복귀가능기간 제공 위해

    string user_id2;
    cout << u8"메세지 삭제할 user_id: ";
    cin >> user_id2;
    select_delete.Delete_Message(user_id2);    // 메세지 삭제

    User_ban user_ban(db.getConnection());
    // 밴 처리
    svr.Put("/chat/admin/ban", [&](const httplib::Request& req, httplib::Response& res) {
        user_ban.handle_user_ban(req, res);
        });

    // 밴해제 처리
    User_ban user_unban(db.getConnection());

    svr.Put("/chat/admin/unban", [&](const httplib::Request& req, httplib::Response& res) {
        user_unban.handle_user_unban(req, res);
        });

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
