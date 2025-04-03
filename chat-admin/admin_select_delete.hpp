#pragma once

#include<iostream>
#include<mysql/jdbc.h>
#include<windows.h>
#include "DB_admin.hpp"
#include <sstream>
#include "json.hpp"

using namespace std;
using namespace sql;


class Select_delete {
private:
    Connection* conn;                                                                       // Connection 타입의 포인터 conn
public:
    Select_delete(Connection* dbconn) : conn(dbconn) {                                     // 의존성 주입. MySQLConnector 객체로부터 주입받음

    }

    // 사용자 정보 조회 후 JSON 반환 (login_id, user_name, user_status만 포함)
    json All_Select() {
        json result_json = json::array();

        try {
            unique_ptr<Statement> stmt(conn->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery("SELECT user_id, login_id, user_name, user_status FROM User"));

            while (res->next()) {

                json user;
                user["user_id"] = res->getInt("user_id");
                user["login_id"] = res->getString("login_id");
                user["user_name"] = res->getString("user_name");
                user["user_status"] = res->getInt("user_status");

                result_json.push_back(user);
            }
        }
        catch (const SQLException& e) {
            cout << "Query failed: " << e.what() << endl;
        }

        return result_json;
    }

    // GET 요청을 처리하여 JSON 응답 반환
   // GET 요청에서는 req.body를 사용할 수 없다고 함, GET 요청에서는 URL 쿼리 매개변수를 활용해야 함.
    void handle_admin_select(const httplib::Request& req, httplib::Response& res) {
        try {
            json users = All_Select();                                                 // MySQL 데이터 조회
            res.set_content(users.dump(), "application/json");                        // JSON 응답 반환
        }
        catch (const SQLException& e) {
            cout << "Query failed: " << e.what() << endl;

        }
    }


    void Update_Status(const int& user_id) {                                        // 회원 삭제
        try {
            // 현재 user_status 확인
            unique_ptr<PreparedStatement> checkStmt(conn->prepareStatement("SELECT user_status FROM User WHERE user_id = ?"));
            checkStmt->setInt(1, user_id);
            unique_ptr<ResultSet> res(checkStmt->executeQuery());

            if (res->next()) {
                int current_status = res->getInt("user_status");
                if (current_status == 2) {
                    cout << u8"이미 삭제된 user_id: " << user_id << endl;
                    return;
                }

                else {
                    // user_status 변경 (회원 삭제 처리)
                    unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE User SET user_status = 2 WHERE user_id = ?"));
                    pstmt->setInt(1, user_id);                                      // 첫번째 물음표 지정
                    int Status_Change = pstmt->executeUpdate();                     // executeUpdate()는 행의 개수를 반환

                    if (Status_Change > 0) {                                        // 삭제할 행이 있으면 실행
                        cout << u8"User ID " << user_id << u8" 상태가 2로 변경되었습니다." << endl;
                    }
                    else {
                        cout << u8"업데이트 실패: user_id " << user_id << u8"을(를) 찾을 수 없음" << endl;
                    }
                    return;
                }
            }
        }
        catch (SQLException& e) {
            cout << u8"실패: " << e.what() << endl;
        }
    }

    void Delete_Message(const int& msg_id) {                                           //  메세지 삭제
        try {
            unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM Message WHERE msg_id = ?"));
            pstmt->setInt(1, msg_id);
            int Message_userid = pstmt->executeUpdate();

            if (Message_userid > 0) {
                cout << u8"메시지가 성공적으로 삭제되었습니다." << endl;
            }
            else {
                cout << u8"삭제할 메시지가 없습니다." << endl;
            }
        }
        catch (SQLException& e) {
            cout << u8"실패: " << e.what() << endl;
        }
    }

    void handle_admin_user_delete(const httplib::Request& req, httplib::Response& res) {                // 회원 삭제 api 연동 
        try {
            json req_json = json::parse(req.body);

            int user_id = req_json["user_id"];

            Update_Status(user_id);
            res.set_content("user delete sucess", "text/plain");
        }
        catch (const SQLException& e) {
            cout << "user_delete failed" << e.what() << endl;
        }
    }

    void handle_amdim_message_delete(const httplib::Request& req, httplib::Response& res) {            // 메세지 삭제 api 연동
        try {
            json req_json = json::parse(req.body);

            int msg_id = req_json["msg_id"];

            Delete_Message(msg_id);
            res.set_content("message delete sucess", "text/plain");
        }
        catch (const SQLException& e) {
            cout << "message_delete failed" << e.what() << endl;
        }
    }

    void Update_Admin_status(const int& user_id) {                                        // 관리자 권한 부여 
        try {
            // 현재 user_status 확인
            unique_ptr<PreparedStatement> checkStmt(conn->prepareStatement("SELECT user_status FROM User WHERE user_id = ?"));
            checkStmt->setInt(1, user_id);
            unique_ptr<ResultSet> res(checkStmt->executeQuery());

            if (res->next()) {
                int current_status = res->getInt("user_status");
                if (current_status == 0) {
                    cout << u8"이미 관리자 권한입니다. " << endl;
                    return;
                }

                else {
                    // user_status 변경 (관리자 권한 처리)
                    unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE User SET user_status = 0 WHERE user_id = ?"));
                    pstmt->setInt(1, user_id);                                      // 첫번째 물음표 지정
                    int Status_Change = pstmt->executeUpdate();                     // executeUpdate()는 행의 개수를 반환

                    if (Status_Change > 0) {                                        // 삭제할 행이 있으면 실행
                        cout << u8"User ID " << user_id << u8" 상태가 0으로 변경되었습니다." << endl;
                    }
                    else {
                        cout << u8"업데이트 실패: user_id " << user_id << u8"을(를) 찾을 수 없음" << endl;
                    }
                    return;
                }
            }
        }
        catch (SQLException& e) {
            cout << u8"실패: " << e.what() << endl;
        }
    }

    void handle_admin_status(const httplib::Request& req, httplib::Response& res) {                // 관리자 권한 부여 api 연동 
        try {
            json req_json = json::parse(req.body);

            int user_id = req_json["user_id"];

            Update_Admin_status(user_id);
            res.set_content("update admin sucess", "text/plain");
        }
        catch (const SQLException& e) {
            cout << "login failed" << e.what() << endl;
        }
    }


    using json = nlohmann::json;

    std::string User_Select(const std::string& login_id) {                                    // 마이페이지(본인 회원정보 조회) - 순서x
        std::ostringstream oss;                                                               // JSON 문자열을 저장할 스트림
        oss << "[";                                                                           // JSON 배열 시작

        try {
            unique_ptr<PreparedStatement> stmt(conn->prepareStatement(
                "SELECT user_id, login_id, login_pw, user_name, user_addr\
                ,user_phone, user_email, user_birthdate FROM User WHERE login_id = ?"
            ));
            stmt->setString(1, login_id);
            unique_ptr<ResultSet> res(stmt->executeQuery());

            bool first = true;
            while (res->next()) {
                if (!first) oss << ",";                                                         // 여러 개의 JSON 객체 구분
                first = false;

                // JSON 객체를 문자열로 직접 구성 (순서 고정)
                oss << "{"
                    << "\"user_id\":" << res->getInt("user_id") << ","
                    << "\"login_id\":\"" << res->getString("login_id") << "\","
                    << "\"login_pw\":\"" << res->getString("login_pw") << "\","
                    << "\"user_name\":\"" << res->getString("user_name") << "\","
                    << "\"user_addr\":\"" << res->getString("user_addr") << "\","
                    << "\"user_phone\":\"" << res->getString("user_phone") << "\","
                    << "\"user_email\":\"" << res->getString("user_email") << "\","
                    << "\"user_birthdate\":\"" << res->getString("user_birthdate") << "\""
                    << "}";
            }
        }
        catch (const SQLException& e) {
            std::cerr << "Query failed: " << e.what() << std::endl;
        }

        oss << "]";                                                                                         // JSON 배열 닫기
        return oss.str();                                                                                   // JSON 문자열 반환
    }
    void handle_user_select(const httplib::Request& req, httplib::Response& res) {                          // user select api 연동 - 순서 x
        try {
            if (req.body.empty()) {
                res.status = 400;
                res.set_content("error: Request body is empty", "application/json");
                return;
            }

            // login_id가 없거나 비어있는지 확인
            auto body_json = json::parse(req.body);
            if (!body_json.contains("login_id") || body_json["login_id"].is_null() || body_json["login_id"].get<std::string>().empty()) {
                res.status = 400;
                res.set_content("error: login_id is required and cannot be empty", "application/json");
                return;
            }

            std::string login_id = body_json["login_id"];
            std::string users_json = User_Select(login_id);

            if (users_json == "[]") {                                                                       // 조회된 결과가 없는 경우
                res.status = 404;
                res.set_content("error: User not found", "application/json");
                return;
            }

            res.set_content(users_json, "application/json");
        }
        catch (const SQLException& e) {
            std::cerr << "user_select failed: " << e.what() << std::endl;
        }
    }
};