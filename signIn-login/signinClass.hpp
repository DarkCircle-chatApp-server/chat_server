#pragma once
#include <iostream>
#include "httplib.h"
#include <string>
#include <mysql/jdbc.h>
#include "DBmodule.hpp"
#include "json.hpp"     // JSON 파싱을 위한 라이브러리. 디스코드 참고할 것
#include "jwt/jwt.hpp"

using namespace std;
using namespace sql;
using json = nlohmann::json;
using namespace jwt::params;

// JWT 발급을 위한 시크릿 키
const string JWT_SECRET_KEY = "secret_key";


// c++에는 date 자료형이 없어서 따로 년-월-일 담을 구조체 생성
struct Birthdate {
    int year;
    int month;
    int day;
};

// 회원가입 데이터 담을 클래스
class SignIn {
private:
    string login_id;
    string login_pw;
    string user_name;
    string user_addr;
    string user_phone;
    int user_status;
    string user_email;
    Birthdate user_birthdate;
    Connection* conn;   // Connection 타입의 포인터 conn
public:
    SignIn(Connection* dbconn) : conn(dbconn) {      // 의존성 주입. MySQLConnector 객체로부터 주입받음

    }
    // 회원테이블 조회(테스트용으로 만들었음)
   /* void show_users() {
        unique_ptr<Statement> stmt{ conn->createStatement() };
        unique_ptr<ResultSet> res{ stmt->executeQuery("SELECT user_name FROM User") };
        while (res->next()) {
            cout << res->getString("user_name") << endl;
        }
    }*/

    // 회원테이블에 회원가입 데이터 삽입
    void insert_user(const string& login_id, const string& login_pw, const string& user_name, const string& user_addr, const string& user_phone, const string& user_email, const Birthdate user_birthdate) {
        try {
            unique_ptr<Statement> stmt{ conn->createStatement() };
            stmt->execute("SET NAMES utf8mb4");

            unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("INSERT INTO User(login_id, login_pw, user_name, user_addr, user_phone, user_email, user_birthdate) values(?, ?, ?, ?, ?, ?, ?)") };

            // birthdate -> YYYY-MM-DD 형식으로 변환
            string birthdate_str = to_string(user_birthdate.year) + "-"
                + (user_birthdate.month < 10 ? "0" : "") + to_string(user_birthdate.month) + "-"
                + (user_birthdate.day < 10 ? "0" : "") + to_string(user_birthdate.day);

            // PreparedStatement에 값 설정
            pstmt->setString(1, login_id);
            pstmt->setString(2, login_pw);
            pstmt->setString(3, user_name);
            pstmt->setString(4, user_addr);
            pstmt->setString(5, user_phone);
            pstmt->setString(6, user_email);
            pstmt->setString(7, birthdate_str);

            int result = pstmt->executeUpdate();
            if (result > 0) {
                cout << u8"회원가입 성공" << endl;
            }
            else {
                cout << u8"회원가입 실패" << endl;
            }
        }

        catch (const SQLException &e) {
            cout << "INSERT Error" << e.what() << endl;
        }
    }   

<<<<<<< Updated upstream
    // JWT 발급 함수
    string create_jwt(const string& login_id, const string& user_email) {
=======
    // JWT 토큰 발급 함수
    string create_jwt(const string& login_id) {
>>>>>>> Stashed changes
        try {
            jwt::jwt_object obj{ algorithm("HS256"), secret(JWT_SECRET_KEY) };
            obj.add_claim("login_id", login_id)
               .add_claim("iss", "auth_service");

            return obj.signature();
        }
        catch (const exception& e) {
            cout << "JWT 생성 오류: " << e.what() << endl;
            return "";
        }
    }

    // 회원가입 처리 함수
    void handle_signIn(const httplib::Request& req, httplib::Response& res) {
        try {

            // 클라이언트의 요청은 json 데이터 타입으로 서버로 들어옴
            // json 객체 req_json 생성
            // req.body : 서버로 들어온 json 타입의 데이터를 string으로 파싱
            json req_json = json::parse(req.body);

            Birthdate birthdate;

            // json 데이터 추출
            string login_id = req_json["login_id"];
            string login_pw = req_json["login_pw"];
            string user_name = req_json["user_name"];
            string user_addr = req_json["user_addr"];
            string user_phone = req_json["user_phone"];
            string user_email = req_json["user_email"];
            birthdate.year = req_json["user_birthdate"]["year"];
            birthdate.month = req_json["user_birthdate"]["month"];
            birthdate.day = req_json["user_birthdate"]["day"];

            // 회원정보 db 삽입
            insert_user(login_id, login_pw, user_name, user_addr, user_phone, user_email, birthdate);

            // JWT 토큰 생성
            string token = create_jwt(login_id);

            // 회원가입 성공하면 클라이언트에 jwt토큰 반환
            // jwt 토큰
            /*{
                "message": "sign-in success",
                "token" : "sdjflsSJALFDJASLDFsjdlfkjSDFJLsdfj1321sdlkfjs123..." // 암호화된 데이터
            }*/
            json response = { {"message", "sign-in success"}, {"token", token} };

            // 처리된 결과 응답 반환
            // response.dump() : json 데이터({ {"message", "sign-in success"}, {"token", token} }) -> 문자열 변환한 값
            res.set_content(response.dump(), "application/json");
        }
        catch (const SQLException& e) {
            cout << "INSERT Error" << e.what() << endl;
        } 
    }

    // 입력데이터와 회원테이블 데이터 비교
    bool check_data(const string& login_id, const string& login_pw) {
        try {
            unique_ptr<Statement> stmt{ conn->createStatement() };
            stmt->execute("SET NAMES utf8mb4");

            unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("SELECT login_pw FROM User WHERE login_id = ?") };
            pstmt->setString(1, login_id);
            unique_ptr<ResultSet> res{ pstmt->executeQuery() };

            if (res->next()) {
                // 회원테이블 login_pw 컬럼값 가져옴
                string chat_password = res->getString("login_pw");
                // 입력한 chat_password와 db에서 가져온 login_pw가 일치하는지 비교(true/false 반환)
                return login_pw == chat_password;
            }
        }
        catch (const SQLException& e) {
            cout << "login failed" << e.what() << endl;
        }
        // 위에서 true 반환 못받으면 false 반환
        return false;
    }

    // 로그인 처리 함수
    void handle_login(const httplib::Request& req, httplib::Response& res) {
        try {
            json req_json = json::parse(req.body);

            string login_id = req_json["login_id"];
            string login_pw = req_json["login_pw"];

            if (check_data(login_id, login_pw)) {
                cout << u8"회원가입 성공" << endl;
                // 아이디 비밀번호 일치 -> 로그인 성공 시 jwt 토큰 반환
                string token = create_jwt(login_id);

                // 로그인 성공하면 클라이언트에 jwt 토큰 반환
                json response = { {"message", "login success"}, {"token", token} };
                // 응답 반환
                res.set_content(response.dump(), "application/json");
            }
            else {
                res.set_content("login failed", "text/plain");
            }
        }
        catch (const SQLException& e) {
            cout << "login failed" << e.what() << endl;
        }
    }
    
    
};

