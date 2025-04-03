#define NDEBUG
#include "httplib.h"    // httplib 헤더파일 추가
#include <iostream>
#include "DBmodule.hpp"
#include "signinClass.hpp"
#include <Windows.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
//#include <nlohmann/json.hpp>  // JSON 파싱을 위한 라이브러리

using namespace std;


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

int main() {
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_library_init();
    SetConsoleOutputCP(CP_UTF8);    // 콘솔 출력 인코딩. 한글입력값 왼쪽에 u8 붙여줄 것
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
	auto connection = db.getConnection();

	SignIn signin(connection);  // getConnection()에서 반환된 MySQLConnector의 conn을 signin객체에 주입;
    //SignIn signin(db.getConnection());  // getConnection()에서 반환된 MySQLConnector의 conn을 signin객체에 주입
    cout << "test start" << endl;
    //signin.get_user_status("admin12345");

    httplib::Server svr;    // httplib::Server 객체 생성

    /*svr.Options("/login", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
    });
    svr.Options("/signIn", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });*/

    // "/login" URL로 들어오는 GET 요청을 handleLogin 함수로 처리
    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        signin.handle_login(req, res);
    });
    
    svr.Post("/idCheck", [&](const httplib::Request& req, httplib::Response& res) {
        signin.check_validation(req, res);
    });

    svr.Get("/statCheck/:login_id", [&](const httplib::Request& req, httplib::Response& res) {
        signin.check_user_status(req, res);
    });

    svr.Get("/getName/:login_id", [&](const httplib::Request& req, httplib::Response& res) {
        signin.show_name(req, res);
    });

    svr.Get("/showId/:login_id", [&](const httplib::Request& req, httplib::Response& res) {
        signin.show_id(req, res);
    });

    // 아이디 중복 체크
    /*svr.Post("/idCheck", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "Request received at /idCheck" << endl;
        try {
            if (!signin.check_validation(req, res)) {
                cout << "ID already exists, 400 response sent" << endl;
                return;
            }
            res.status = 200;
            res.set_content("ID is available", "text/plain");
            cout << "ID is available, 200 response sent" << endl;
        }
        catch (const std::exception& e) {
            res.status = 500;
            res.set_content("Internal Server Error: " + string(e.what()), "text/plain");
            cerr << "Error: " << e.what() << endl;
        }
    });*/


    // "/signIn" URL로 들어오는 POST 요청을 handleSignIn 함수로 처리
    // 람다식 쓴 이유 : 람다 안쓰면 signin.handleSignIn() 이런식으로 외부 객체 함수에 접근 못한다고 함..시발
    // [&] : 캡처 리스트 - 현재 스코프의 변수들을 참조 방식으로 캡처(설명에 이렇게 나와있는데 그냥 람다식 앞에 붙이는 코드인듯함)
    svr.Post("/signIn", [&](const httplib::Request& req, httplib::Response& res) {
        //std::cout << "Request body: " << req.body << std::endl;
        signin.handle_signIn(req, res);
    });

    // 회원 탈퇴 요청
    svr.Put("/update2", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "call success" << endl;
        signin.handle_delete(req, res);
    });

    // CORS 설정(8080 포트에서 들어오는 요청 허용)
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    // 5001번 포트로 들어오는 클라이언트 요청 받음
    std::cout << "Login Service running: http://localhost:8880" << std::endl;
    std::cout << "SignIn Service running: http://localhost:8880" << std::endl;
    svr.listen("0.0.0.0", 8880);

    //svr.listen("0.0.0.0", 5002);

    // return 0; 하면 안 됨 return 실행되면 프로그램 종료됨. 서버 실행된 채로 계속 유지. 종료는 우리가 필요할 때 해야함
}

