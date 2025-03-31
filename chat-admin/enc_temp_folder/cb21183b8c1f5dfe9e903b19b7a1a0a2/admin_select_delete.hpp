#pragma once

#include<iostream>
#include<mysql/jdbc.h>
#include<windows.h>
#include <ctime> // 시간
#include<time.h>
#include <memory>
#include <thread>
#include "DB_admin.hpp"


using namespace std;
using namespace sql;


class Select_delete {
private:
    Connection* conn;   // Connection 타입의 포인터 conn
public:
    Select_delete (Connection* dbconn) : conn(dbconn) {      // 의존성 주입. MySQLConnector 객체로부터 주입받음

    }

    //void All_Select() {
    //    try {
    //        unique_ptr<Statement> stmt(conn->createStatement());
    //        unique_ptr<ResultSet> res(stmt->executeQuery("SELECT* FROM User"));

    //        cout << "user table: " << endl;
    //        while (res->next()) {
    //            cout << res->getString("user_id") << " "
    //                << res->getString("login_id") << " "
    //                << res->getString("login_pw") << " "
    //                << res->getString("user_name") << " "
    //                << (res->isNull("user_addr") ? "null" : res->getString("user_addr").c_str()) << " " <<
    //                (res->isNull("user_phone") ? "null" : res->getString("user_phone").c_str()) << " "
    //                << res->getString("user_status") << " "
    //                << res->getString("user_email") << " "
    //                << res->getString("user_birthdate") << endl;
    //        }
    //    }
    //    catch (SQLException& e) {
    //        cout << "Query failed: " << e.what() << endl;
    //    }
    //}
            // 사용자 정보 조회 후 JSON 반환 (login_id, user_name, user_status만 포함)
    json All_Select() {
        json result_json = json::array();

        try {
            unique_ptr<Statement> stmt(conn->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery("SELECT login_id, user_name, user_status FROM User"));

            while (res->next()) {
                json user;
                user["login_id"] = res->getString("login_id");
                user["user_name"] = res->getString("user_name");
                user["user_status"] = res->getString("user_status");

                result_json.push_back(user);
            }
        }
        catch (const SQLException& e) {
            cout << "Query failed: " << e.what() << endl;
        }

        return result_json;
    }

    // GET 요청을 처리하여 JSON 응답 반환
    void handle_admin_select(const httplib::Request& req, httplib::Response& res) {
        try {
            json users = All_Select();  // MySQL 데이터 조회
            res.set_content(users.dump(), "application/json");  // JSON 응답 반환
        }
        catch (const SQLException& e) {
            cout << "Query failed: " << e.what() << endl;
            res.status = 500;
            res.set_content("{\"error\": \"Database query failed\"}", "application/json");
        }
    }
    void Update_Status(const int& user_id) {
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
                    pstmt->setInt(1, user_id);   // 첫번째 물음표 지정
                    int Status_Change = pstmt->executeUpdate(); // executeUpdate()는 행의 개수를 반환

                    if (Status_Change > 0) { // 삭제할 행이 있으면 실행
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

    void Delete_Message(const int& msg_id) {      //  메세지 삭제
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
    // GET 요청에서는 req.body를 사용할 수 없다고 함, GET 요청에서는 URL 쿼리 매개변수를 활용해야 함.
    //void handle_admin_select(const httplib::Request& req, httplib::Response& res) {
    //    try {
    //        json req_json = json::parse(req.body);

    //        All_Select();
    //        res.set_content("Select sucess", "text/plain");
    //    }
    //    catch (const SQLException& e) {
    //        cout << "login failed" << e.what() << endl;
    //    }
    //}
    void handle_admin_user_delete(const httplib::Request& req, httplib::Response& res) {
        try {
            json req_json = json::parse(req.body);

            int user_id = req_json["user_id"];

            Update_Status(user_id);
            res.set_content("user delete sucess", "text/plain");
        }
        catch (const SQLException& e) {
            cout << "login failed" << e.what() << endl;
        }
    }
    void handle_amdim_message_delete(const httplib::Request& req, httplib::Response& res) {
        try {
            json req_json = json::parse(req.body);

            int msg_id = req_json["msg_id"];

            Delete_Message(msg_id);
            res.set_content("message delete sucess", "text/plain");
        }
        catch (const SQLException& e) {
            cout << "login failed" << e.what() << endl;
        }
    }


};