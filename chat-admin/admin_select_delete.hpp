#pragma once

#include<iostream>
#include<mysql/jdbc.h>
#include<windows.h>
#include <ctime> // �ð�
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
    Connection* conn;   // Connection Ÿ���� ������ conn
public:
    Select_delete (Connection* dbconn) : conn(dbconn) {      // ������ ����. MySQLConnector ��ü�κ��� ���Թ���

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
            // ���� user_status Ȯ��
            unique_ptr<PreparedStatement> checkStmt(conn->prepareStatement("SELECT user_status FROM User WHERE user_id = ?"));
            checkStmt->setString(1, user_id);
            unique_ptr<ResultSet> res(checkStmt->executeQuery());

            if (res->next()) {
                int current_status = res->getInt("user_status");
                if (current_status == 2) {
                    cout << u8"�̹� ������ user_id: " << user_id << endl;
                    return;
                }

                else {
                    // user_status ���� (ȸ�� ���� ó��)
                    unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE User SET user_status = 2 WHERE user_id = ?"));
                    pstmt->setString(1, user_id);   // ù��° ����ǥ ����
                    int Status_Change = pstmt->executeUpdate(); // executeUpdate()�� ���� ������ ��ȯ

                    if (Status_Change > 0) { // ������ ���� ������ ����
                        cout << u8"User ID " << user_id << u8" ���°� 2�� ����Ǿ����ϴ�." << endl;
                    }
                    else {
                        cout << u8"������Ʈ ����: user_id " << user_id << u8"��(��) ã�� �� ����" << endl;
                    }
                    return;
                }
            }
        }
        catch (SQLException& e) {
            cout << u8"����: " << e.what() << endl;
        }
    }

    void Delete_Message(const string& user_id) {      //  �޼��� ����
        try {
            unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM Message WHERE user_id = ?"));
            pstmt->setString(1, user_id);
            int Message_userid = pstmt->executeUpdate();

            if (Message_userid > 0) {
                cout << u8"�޽����� ���������� �����Ǿ����ϴ�." << endl;
            }
            else {
                cout << u8"������ �޽����� �����ϴ�." << endl;
            }
        }
        catch (SQLException& e) {
            cout << u8"����: " << e.what() << endl;
        }
    }
 

    void DelayedResetStatus(string user_id) {
        this_thread::sleep_for(chrono::seconds(2));         // 2�� 
        try {
            unique_ptr<PreparedStatement> resetStmt(conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?"));
            resetStmt->setString(1, user_id);
            int Reset_Change = resetStmt->executeUpdate();

            if (Reset_Change > 0) {
                cout << "\n" << u8"User ID " << user_id << u8" ���°� 1���� �����Ǿ����ϴ�." << endl;
            }
            else {
                cout << "\n" << u8"���� ����: user_id " << user_id << u8" ã�� �� ����" << endl;
            }
        }
        catch (SQLException& e) {
            cout << "\n" << u8"���� ����: " << e.what() << endl;
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
                    cout << u8"�̹� ������ user_id: " << user_id << endl;
                    return;
                }

                unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE User SET user_status = 2 WHERE user_id = ?"));
                pstmt->setString(1, user_id);
                int Status_Change = pstmt->executeUpdate();

                if (Status_Change > 0) {
                    cout << u8"User ID " << user_id << u8" ���°� 2�� ����Ǿ����ϴ�." << endl;
                    thread resetThread(&Select_delete::DelayedResetStatus, this, user_id);
                    resetThread.detach(); // ȣ�� ��� ������ ���ҽ�(�޸� ����)�� ����
                }
                else {
                    cout << u8"������Ʈ ����: user_id " << user_id << u8"��(��) ã�� �� ����" << endl;
                }
            }
        }
        catch (SQLException& e) {
            cout << u8"����: " << e.what() << endl;
        }
    }

};