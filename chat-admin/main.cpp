#include "httplib.h"
#include <iostream>

// 채팅 관리자 기능 함수
void handleChatAdmin(const httplib::Request& req, httplib::Response& res) {

    res.set_content("chat admin", "text/plain"); // 성공 응답
}

int main() {
    httplib::Server svr;    // httplib::Server 객체 생성

    svr.Get("/chat/admin", handleChatAdmin);

    // CORS 설정
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << "Chat Service 실행 중: http://localhost:5004" << std::endl;
    svr.listen("0.0.0.0", 5004); // 서버 실행

    // return 0; 하면 안 됨, 서버는 종료될 때까지 계속 실행되어야 함
}
