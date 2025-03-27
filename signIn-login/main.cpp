#include "httplib.h"    // httplib 헤더파일 추가
#include <iostream>
#include "DBmodule.hpp"
#include "signinClass.hpp"
#include <Windows.h>
//#include <nlohmann/json.hpp>  // JSON 파싱을 위한 라이브러리


// 필요한 함수들은 헤더파일에 만들기
// 일단 전부다 간단한 GET 요청으로만 api를 만들어놨음. 기능에 따라 GET, POST, PUT, DELETE 중 적절한 메서드로 변경할 것
// rest api는 url에 포함된 요소들로 데이터에 접근해야함
// 메시지 전송 api 예시) /chat/1(userid임. 동적으로 불러와야함)/sendMessage
// 이런식으로 url에 요소들을 넣어서 컴파일러가 인식하여 처리하도록 만들어야함
// 즉 svr.POST("/chat/{userid}/sendMessage", [](const httplib::Request& req, httplib::Response& res)) 이런식으로 메시지 전송 api를 만드는거임
// userid는 mysql db 데이터 또는 로그인세션에서 가져와야함
// 나도 아직 할 줄 모름. 다들 열심히 구글링해봅시다...!

// 일단 지금 편의상 main.cpp에 함수도 정의 해놨음. 나중에 헤더파일로 옮겨야함
// 헤더파일 예시는 지금 이 프로젝트 안의 test1.hpp, test2.hpp 헤더파일 확인할 것
// 로그인 처리 함수
void handleLogin(const httplib::Request& req, httplib::Response& res) {     // Request& req: 클라이언트의 요청, 
                                                                            // httplib::Response& res: 서버 실행결과 응답 객체
    // 여기에 db연동해서 데이터 처리하는거랑
    // redis로 요청보내는 코드 넣어야함

    // 처리된 결과 응답 반환
    res.set_content("login success", "text/plain");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);    // 콘솔 출력 인코딩. 한글입력값 왼쪽에 u8 붙여줄 것
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    SignIn signin(db.getConnection());  // getConnection()에서 반환된 MySQLConnector의 conn을 signin객체에 주입

    httplib::Server svr;    // httplib::Server 객체 생성

    // "/login" URL로 들어오는 GET 요청을 handleLogin 함수로 처리
    svr.Get("/login", handleLogin);

    // "/signIn" URL로 들어오는 POST 요청을 handleSignIn 함수로 처리
    // 람다식 쓴 이유 : 람다 안쓰면 signin.handleSignIn() 이런식으로 외부 객체 함수에 접근 못한다고 함..시발
    // [&] : 캡처 리스트 - 현재 스코프의 변수들을 참조 방식으로 캡처(설명에 이렇게 나와있는데 그냥 람다식 앞에 붙이는 코드인듯함)
    svr.Post("/signIn", [&](const httplib::Request& req, httplib::Response& res) {
        signin.handleSignIn(req, res);
    });

    // CORS 설정(다른 포트번호에서(react 포트:3000) 들어오는 요청 허용)
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    // 5001번 포트로 들어오는 클라이언트 요청 받음
    std::cout << "Login Service running: http://localhost:5001" << std::endl;
    std::cout << "SignIn Service running: http://localhost:5001" << std::endl;
    svr.listen("0.0.0.0", 5001);

    //svr.listen("0.0.0.0", 5002);

    // return 0; 하면 안 됨 return 실행되면 프로그램 종료됨. 서버 실행된 채로 계속 유지. 종료는 우리가 필요할 때 해야함
}

