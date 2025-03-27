#pragma once
#include <iostream>
#include "httplib.h"
#include <string>
#include <mysql/jdbc.h>
#include "DBmodule.hpp"
#include "json.hpp"     // JSON �Ľ��� ���� ���̺귯��. ���ڵ� ������ ��

using namespace std;
using namespace sql;
using json = nlohmann::json;

// c++���� date �ڷ����� ��� ���� ��-��-�� ���� ����ü ����
struct Birthdate {
    int year;
    int month;
    int day;
};

// ȸ������ ������ ���� Ŭ����
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
    Connection* conn;   // Connection Ÿ���� ������ conn
public:
    SignIn(Connection* dbconn) : conn(dbconn) {      // ������ ����. MySQLConnector ��ü�κ��� ���Թ���

    }
    // ȸ�����̺� ��ȸ(�׽�Ʈ������ �������)
    void showUsers() {
        unique_ptr<Statement> stmt{ conn->createStatement() };
        unique_ptr<ResultSet> res{ stmt->executeQuery("SELECT user_name FROM User") };
        while (res->next()) {
            cout << res->getString("user_name") << endl;
        }
    }
    // ȸ�����̺� ȸ������ ������ ����
    void insertUser(const string& login_id, const string& login_pw, const string& user_name, const string& user_addr, const string& user_phone, const string& user_email, const Birthdate user_birthdate) {
        try {
            unique_ptr<Statement> stmt{ conn->createStatement() };
            stmt->execute("SET NAMES utf8mb4");

            unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("INSERT INTO User(login_id, login_pw, user_name, user_addr, user_phone, user_email, user_birthdate) values(?, ?, ?, ?, ?, ?, ?)") };

            // birthdate -> YYYY-MM-DD �������� ��ȯ
            string birthdate_str = to_string(user_birthdate.year) + "-"
                + (user_birthdate.month < 10 ? "0" : "") + to_string(user_birthdate.month) + "-"
                + (user_birthdate.day < 10 ? "0" : "") + to_string(user_birthdate.day);

            // PreparedStatement�� �� ����
            pstmt->setString(1, login_id);
            pstmt->setString(2, login_pw);
            pstmt->setString(3, user_name);
            pstmt->setString(4, user_addr);
            pstmt->setString(5, user_phone);
            pstmt->setString(6, user_email);
            pstmt->setString(7, birthdate_str);

            int result = pstmt->executeUpdate();
            if (result > 0) {
                cout << "ȸ������ ����" << endl;
            }
            else {
                cout << "ȸ������ ����" << endl;
            }
        }

        catch (const SQLException &e) {
            cout << "INSERT Error" << e.what() << endl;
        }
    }

    // ȸ������ ó�� �Լ�
    void handleSignIn(const httplib::Request& req, httplib::Response& res) {
        try {

            // Ŭ���̾�Ʈ�� ��û�� json ������ Ÿ������ ������ ����
            // json ��ü req_json ����
            // req.body : ������ ���� json Ÿ���� �����͸� string���� �Ľ�
            json req_json = json::parse(req.body);

            Birthdate birthdate;

            // json ������ ����
            string login_id = req_json["login_id"];
            string login_pw = req_json["login_pw"];
            string user_name = req_json["user_name"];
            string user_addr = req_json["user_addr"];
            string user_phone = req_json["user_phone"];
            string user_email = req_json["user_email"];
            birthdate.year = req_json["user_birthdate"]["year"];
            birthdate.month = req_json["user_birthdate"]["month"];
            birthdate.day = req_json["user_birthdate"]["day"];

            // ȸ������ db ����
            insertUser(login_id, login_pw, user_name, user_addr, user_phone, user_email, birthdate);

            // ó���� ��� ���� ��ȯ
            res.set_content("sign-in success", "text/plain");
        }
        catch (const SQLException& e) {
            cout << "INSERT Error" << e.what() << endl;
        } 

    }
};

