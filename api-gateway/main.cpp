#include "httplib.h"
#include <iostream>

// 각 핸들러 함수를 정의

// 기본 API Gateway 엔드포인트
void handleRoot(const httplib::Request&, httplib::Response& res) {
    res.set_content("API Gateway is running", "text/plain");
}

// test1 → 5001번 포트의 login 호출
void handleTest1(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5001");
    auto response = cli.Get("/login");
    if (response) {
        res.set_content(response->body, "text/plain");
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_content("Error fetching data from /login", "text/plain");
    }
}

// test2 → 5002번 포트의 signIn 호출
void handleTest2(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5002");
    auto response = cli.Get("/signIn");
    if (response) {
        res.set_content(response->body, "text/plain");
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_content("Error fetching data from /signIn", "text/plain");
    }
}

// test3 → 5003번 포트의 chat 호출
void handleTest3(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5003");
    auto response = cli.Get("/chat");
    if (response) {
        res.set_content(response->body, "text/plain");
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_content("Error fetching data from /chat", "text/plain");
    }
}

// test4 → 5003번 포트의 chat admin 호출
void handleTest4(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5003");
    auto response = cli.Get("/chat/admin");
    if (response) {
        res.set_content(response->body, "text/plain");
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_content("Error fetching data from /chat/admin", "text/plain");
    }
}

int main() {
    httplib::Server svr;

    // "/" 경로에 대해 handleRoot 함수 연결
    svr.Get("/", handleRoot);

    // "/test1" 경로에 대해 handleTest1 함수 연결
    svr.Get("/test1", handleTest1);

    // "/test2" 경로에 대해 handleTest2 함수 연결
    svr.Get("/test2", handleTest2);

    // "/test3" 경로에 대해 handleTest3 함수 연결
    svr.Get("/test3", handleTest3);

    // "/test4" 경로에 대해 handleTest4 함수 연결
    svr.Get("/test4", handleTest4);

    // CORS 설정
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << "API Gateway 실행 중: http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    // 서버가 실행된 상태로 계속 유지
}
