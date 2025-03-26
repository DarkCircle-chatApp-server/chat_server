
// test4

// github test1

// github test3

// github test2

#include "httplib.h"
#include <iostream>


// 채팅 관련 함수
void handleChat(const httplib::Request& req, httplib::Response& res) {
    
    // 내부 로직 기능

    res.set_content("chat", "text/plain");
}

int main() {
    httplib::Server svr;    // httplib::Server 객체 생성

    svr.Get("/chat", handleChat);

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
