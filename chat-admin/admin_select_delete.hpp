#pragma once

#include<iostream>
#include<mysql/jdbc.h>
#include<windows.h>
#include <ctime> // 시간
#include<time.h>
#include <memory>
#include <thread>
#include "prepared_statement.h"
#include "resultset.h"
#include "exception.h"
#include "DB.hpp"


using namespace std;
using namespace sql;


class Select_delete {
private:
    Connection* conn;   // Connection 타입의 포인터 conn
public:
    Select_delete (Connection* dbconn) : conn(dbconn) {      // 의존성 주입. MySQLConnector 객체로부터 주입받음

    }

    void All_Select() {
        try {
            unique_ptr<Statement> stmt(conn->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery("SELECT* FROM User"));

            cout << "user table: " << endl;
            while (res->next()) {
                cout << res->getString("user_id") << " "
                    << res->getString("login_id") << " "
                    << res->getString("login_pw") << " "
                    << res->getString("user_name") << " "
                    << (res->isNull("user_addr") ? "null" : res->getString("user_addr").c_str()) << " " <<
                    (res->isNull("user_phone") ? "null" : res->getString("user_phone").c_str()) << " "
                    << res->getString("user_status") << " "
                    << res->getString("user_email") << " "
                    << res->getString("user_birthdate") << endl;
            }
        }
        catch (SQLException& e) {
            cout << "Query failed: " << e.what() << endl;
        }
    }
    void Update_Status(const string& user_id) {
        try {
            // 현재 user_status 확인
            unique_ptr<PreparedStatement> checkStmt(conn->prepareStatement("SELECT user_status FROM User WHERE user_id = ?"));
            checkStmt->setString(1, user_id);
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
                    pstmt->setString(1, user_id);   // 첫번째 물음표 지정
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

    void Delete_Message(const string& user_id) {      //  메세지 삭제
        try {
            unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM Message WHERE user_id = ?"));
            pstmt->setString(1, user_id);
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
 

    void DelayedResetStatus(string user_id) {
        this_thread::sleep_for(chrono::seconds(2));         // 2초 
        try {
            unique_ptr<PreparedStatement> resetStmt(conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?"));
            resetStmt->setString(1, user_id);
            int Reset_Change = resetStmt->executeUpdate();

            if (Reset_Change > 0) {
                cout << "\n" << u8"User ID " << user_id << u8" 상태가 1으로 복구되었습니다." << endl;
            }
            else {
                cout << "\n" << u8"복구 실패: user_id " << user_id << u8" 찾을 수 없음" << endl;
            }
        }
        catch (SQLException& e) {
            cout << "\n" << u8"복구 실패: " << e.what() << endl;
        }
    }

    void Update_Status2(const string& user_id) {
        try {
            unique_ptr<PreparedStatement> checkStmt(conn->prepareStatement("SELECT user_status FROM User WHERE user_id = ?"));
            checkStmt->setString(1, user_id);
            unique_ptr<ResultSet> res(checkStmt->executeQuery());

            if (res->next()) {
                int current_status = res->getInt("user_status");
                if (current_status == 2) {
                    cout << u8"이미 삭제된 user_id: " << user_id << endl;
                    return;
                }

                unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE User SET user_status = 2 WHERE user_id = ?"));
                pstmt->setString(1, user_id);
                int Status_Change = pstmt->executeUpdate();

                if (Status_Change > 0) {
                    cout << u8"User ID " << user_id << u8" 상태가 2로 변경되었습니다." << endl;
                    thread resetThread(&Select_delete::DelayedResetStatus, this, user_id);
                    resetThread.detach(); // 호출 즉시 스레드 리소스(메모리 영역)를 해제
                }
                else {
                    cout << u8"업데이트 실패: user_id " << user_id << u8"을(를) 찾을 수 없음" << endl;
                }
            }
        }
        catch (SQLException& e) {
            cout << u8"실패: " << e.what() << endl;
        }
    }

};