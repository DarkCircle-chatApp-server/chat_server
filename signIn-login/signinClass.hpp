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
    SignIn(Connection* dbconn) : conn(move(dbconn)) {      // 의존성 주입. MySQLConnector 객체로부터 주입받음.
                                                           // 복사하지 않고 이동생성자로 초기화
    }
    ~SignIn() {
    }
    // user_id 조회
    int get_key(const string& login_id) {
        if (!conn) {
            cerr << "Database connection is not initialized!" << endl;
            return -1;
        }
        try {
            unique_ptr<PreparedStatement> stmt{ conn->prepareStatement("SELECT user_id FROM User WHERE login_id = ?") };
            stmt->setString(1, login_id);
            unique_ptr<ResultSet> res{ stmt->executeQuery() };

            if (res->next()) {
                int user_id = res->getInt(1);
                cout << user_id << endl;
                return user_id;
            }
            else {
                cerr << "No users" << endl;
                return -1;
            }
        }
        catch (const SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
            return -1;
        }
    }

    void show_id(const httplib::Request& req, httplib::Response& res) {
        if (req.path_params.find("login_id") == req.path_params.end()) {
            res.status = 400;
            res.set_content("{\"error\": \"user_name이 없습니다.\"}", "application/json");
            return;
        }
        string login_id = req.path_params.at("login_id");  // api url에서 login_id 추출
        int user_id = get_key(login_id);

        if (user_id != -1) {
            cout << "request success" << endl;
            string json_response = "{\"user_id\": " + to_string(user_id) + "}";
            res.set_content(json_response, "application/json");
        }
        else {
            res.status = 404;
            res.set_content("{\"error\": \"No users\"}", "application/json");
        }
    }

    string get_name(const string& login_id) {
        if (!conn) {
            cerr << "Database connection is not initialized!" << endl;
            return "";
        }
        try {
            unique_ptr<PreparedStatement> stmt{ conn->prepareStatement("SELECT user_name FROM User WHERE login_id = ?") };
            stmt->setString(1, login_id);
            unique_ptr<ResultSet> res{ stmt->executeQuery() };

            if (res->next()) {
                string user_name = res->getString(1);
                cout << user_name << endl;
                return user_name;
            }
            else {
                cerr << "No users" << endl;
                return "Unknown";
            }
        }
        catch (const SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
            return "Unknown";
        }
        catch (const std::exception& e) {
            cerr << "Exception: " << e.what() << std::endl;
            return "Unknown";
        }
    }

    void show_name(const httplib::Request& req, httplib::Response& res) {
        if (req.path_params.find("login_id") == req.path_params.end()) {
            res.status = 400;
            res.set_content("{\"error\": \"user_name이 없습니다.\"}", "application/json");
            return;
        }
        string login_id = req.path_params.at("login_id");  // api url에서 login_id 추출
        string user_name = get_name(login_id);

        if (user_name != "Unknown") {
            cout << "request success" << endl;
            string json_response = "{\"user_name\": \"" + user_name + "\"}";
            res.set_content(json_response, "application/json");
        }
        else {
            res.status = 404;
            res.set_content("{\"error\": \"No users\"}", "application/json");
        }
    }

    // user_status값 조회
    int get_user_status(const string& login_id) {
        if (!conn) {
            cerr << "Database connection is not initialized!" << endl;
            return -1;
        }
        try {
            if (!conn) {
                cerr << "Database connection is not initialized!" << endl;
                return -1;
            }
            unique_ptr<PreparedStatement> stmt{ conn->prepareStatement("SELECT user_status FROM User WHERE login_id = ?") };
            stmt->setString(1, login_id);
            unique_ptr<ResultSet> res{stmt->executeQuery()};

            if (res->next()) {
                //cout << "test success" << endl;
                int user_status = res->getInt(1);
                cout << user_status << endl;
                return user_status;
            }
            else {
                cerr << "No users" << endl;
                return -1;
            }
        }
        catch (const SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
            return -1;
        }
    }

    void check_user_status(const httplib::Request& req, httplib::Response& res) {
        if (req.path_params.find("login_id") == req.path_params.end()) {
            res.status = 400;
            res.set_content("{\"error\": \"login_id가 없습니다.\"}", "application/json");
            return;
        }
        string login_id = req.path_params.at("login_id");  // api url에서 login_id 추출
        int user_status = get_user_status(login_id);

        if (user_status != -1) {
            cout << "request success" << endl;
            string json_response = "{\"user_status\": " + to_string(user_status) + "}";
            res.set_content(json_response, "application/json");
        }
        else {
            res.status = 404;
            res.set_content("{\"error\": \"No users\"}", "application/json");
        }
    }
    
    // 아이디 중복 체크
    bool is_id_exist(const string& login_id) {
        if (!conn) {
            cerr << "Database connection is not initialized!" << endl;
            return -1;
        }
        try {
            unique_ptr<PreparedStatement> stmt{ conn->prepareStatement("SELECT COUNT(*) FROM User WHERE login_id = ?") };
            stmt->setString(1, login_id);
            unique_ptr<ResultSet> res{ stmt->executeQuery() };

            if (res->next()) {
                cout << "is_id_exist run" << endl;
                cout << "login_id: " << login_id << ", res->getInt(1): " << res->getInt(1) << endl;
                return res->getInt(1) != 0;  // getInt(2) -> login_id에 하나라도 존재하면 1, 존재안하면 0 // 만약 존재하면 true 반환
            }
        }
        catch (const SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
        // 예외 발생 시 false 반환
        return false;
    }

    bool check_validation(const httplib::Request& req, httplib::Response& res) {
        json req_json = json::parse(req.body);

        // json 데이터 추출
        string login_id = req_json["login_id"];
        //string login_pw = req_json["login_pw"];

        if (is_id_exist(login_id)) {
            //cout << "check_validation run : " << is_id_exist(login_id) << endl;
            res.status = 400;   // 아이디 존재(true)면 400응답이 떠서 요청 실행이 안되도록
            res.set_content(u8"ID already exists", "text/plain");
            cout << "current status: " << res.status << endl;
            return false;
        }
        return true;
    }

    // 회원테이블에 회원가입 데이터 삽입
    void insert_user(const string& login_id, const string& login_pw, const string& user_name, const string& user_addr, const string& user_phone, const string& user_email, const Birthdate user_birthdate) {
        if (!conn) {
            cerr << "Database connection is not initialized!" << endl;
        }
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
                cout << "sign-in success" << endl;
            }
            else {
                cout << "sign-in failed" << endl;
            }
        }

        catch (const SQLException &e) {
            cout << "INSERT Error" << e.what() << endl;
        }
    }   

    // JWT 토큰 발급 함수
    string create_jwt(const string& login_id) {
        try {
            jwt::jwt_object obj{ algorithm("HS256"), secret(JWT_SECRET_KEY) };
            obj.add_claim("login_id", login_id)
               .add_claim("iss", "auth_service");

            return obj.signature();
        }
        catch (const exception& e) {
            cout << "JWT Creation Error: " << e.what() << endl;
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
            if (!check_validation(req, res)) {
                cout << "Validation failed. Status: " << res.status << endl;
                //res.set_content("ID already exists2", "text/plain");
                return;
            }
            insert_user(login_id, login_pw, user_name, user_addr, user_phone, user_email, birthdate);
            // JWT 토큰 생성
            string token = create_jwt(login_id);
            json response = { {"message", "sign-in success"}, {"token", token} };
            res.set_content(response.dump(), "application/json");
            res.status = 200;
            //return 200;

            // 회원가입 성공하면 클라이언트에 jwt토큰 반환
            // jwt 토큰
            /*{
                "message": "sign-in success",
                "token" : "sdjflsSJALFDJASLDFsjdlfkjSDFJLsdfj1321sdlkfjs123..." // 암호화된 데이터
            }*/

            // 처리된 결과 응답 반환
            // response.dump() : json 데이터({ {"message", "sign-in success"}, {"token", token} }) -> 문자열 변환한 값
        }
        catch (const SQLException& e) {
            cout << "INSERT Error" << e.what() << endl;
            res.set_content("Internal server error", "text/plain");
            res.status = 500;
        }
        //return 500;
    }

    // 입력데이터와 회원테이블 데이터 비교
    bool check_data(const string& login_id, const string& login_pw) {
        if (!conn) {
            cerr << "Database connection is not initialized!" << endl;
            return -1;
        }
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
			int user_id = get_key(login_id);

            if (check_data(login_id, login_pw)) {
                cout << "login success" << endl;
                // 아이디 비밀번호 일치 -> 로그인 성공 시 jwt 토큰 반환
                string token = create_jwt(login_id);

                // 로그인 성공하면 클라이언트에 jwt 토큰 반환
                json response = { {"message", "login success"}, {"token", token}, {"login_id", login_id}, {"user_id", user_id}};
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

    // user_status 2로 변경 -> 관리자 승인 후 최종 탈퇴 처리
    void delete_user(const int& user_id) {
        if (!conn) {
            cerr << "Database connection is not initialized!" << endl;
        }
        try {
            unique_ptr<Statement> stmt{ conn->createStatement() };
            stmt->execute("SET NAMES utf8mb4");

            unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("UPDATE User SET user_status = 2 WHERE user_id = ?") };

            pstmt->setInt(1, user_id);

            int result = pstmt->executeUpdate();
            if (result > 0) {
                cout << "need to wait admin's acception" << endl;
            }
            else {
                cout << "deletion requirement failed" << endl;
            }
        }
        catch (const SQLException &e) {
            cout << "UPDATE Error: " << e.what() << endl;
        }
    }

    // 회원 탈퇴 요청
    void handle_delete(const httplib::Request& req, httplib::Response& res) {
        try {
            json req_json = json::parse(req.body);

            int user_id = req_json["user_id"];

            delete_user(user_id);

            json response = { {"message", "UPDATE success"} };
            res.set_content(response.dump(), "application/json");

        }
        catch (const SQLException& e) {
            cout << "UPDATE Error: " << e.what() << endl;
        }
    }
    
    
};

