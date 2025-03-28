#pragma once
#include <iostream>
#include "httplib.h"
#include <string>
#include <mysql/jdbc.h>
#include "DBmodule.hpp"
#include "json.hpp"     // JSON �Ľ��� ���� ���̺귯��. ���ڵ� ������ ��
#include "jwt/jwt.hpp"

using namespace std;
using namespace sql;
using json = nlohmann::json;
using namespace jwt::params;

// JWT �߱��� ���� ��ũ�� Ű
const string JWT_SECRET_KEY = "secret_key";


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
   /* void show_users() {
        unique_ptr<Statement> stmt{ conn->createStatement() };
        unique_ptr<ResultSet> res{ stmt->executeQuery("SELECT user_name FROM User") };
        while (res->next()) {
            cout << res->getString("user_name") << endl;
        }
    }*/

    // ȸ�����̺� ȸ������ ������ ����
    void insert_user(const string& login_id, const string& login_pw, const string& user_name, const string& user_addr, const string& user_phone, const string& user_email, const Birthdate user_birthdate) {
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
                cout << u8"ȸ������ ����" << endl;
            }
            else {
                cout << u8"ȸ������ ����" << endl;
            }
        }

        catch (const SQLException &e) {
            cout << "INSERT Error" << e.what() << endl;
        }
    }   

<<<<<<< Updated upstream
    // JWT �߱� �Լ�
    string create_jwt(const string& login_id, const string& user_email) {
=======
    // JWT ��ū �߱� �Լ�
    string create_jwt(const string& login_id) {
>>>>>>> Stashed changes
        try {
            jwt::jwt_object obj{ algorithm("HS256"), secret(JWT_SECRET_KEY) };
            obj.add_claim("login_id", login_id)
               .add_claim("iss", "auth_service");

            return obj.signature();
        }
        catch (const exception& e) {
            cout << "JWT ���� ����: " << e.what() << endl;
            return "";
        }
    }

    // ȸ������ ó�� �Լ�
    void handle_signIn(const httplib::Request& req, httplib::Response& res) {
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
            insert_user(login_id, login_pw, user_name, user_addr, user_phone, user_email, birthdate);

            // JWT ��ū ����
            string token = create_jwt(login_id);

            // ȸ������ �����ϸ� Ŭ���̾�Ʈ�� jwt��ū ��ȯ
            // jwt ��ū
            /*{
                "message": "sign-in success",
                "token" : "sdjflsSJALFDJASLDFsjdlfkjSDFJLsdfj1321sdlkfjs123..." // ��ȣȭ�� ������
            }*/
            json response = { {"message", "sign-in success"}, {"token", token} };

            // ó���� ��� ���� ��ȯ
            // response.dump() : json ������({ {"message", "sign-in success"}, {"token", token} }) -> ���ڿ� ��ȯ�� ��
            res.set_content(response.dump(), "application/json");
        }
        catch (const SQLException& e) {
            cout << "INSERT Error" << e.what() << endl;
        } 
    }

    // �Էµ����Ϳ� ȸ�����̺� ������ ��
    bool check_data(const string& login_id, const string& login_pw) {
        try {
            unique_ptr<Statement> stmt{ conn->createStatement() };
            stmt->execute("SET NAMES utf8mb4");

            unique_ptr<PreparedStatement> pstmt{ conn->prepareStatement("SELECT login_pw FROM User WHERE login_id = ?") };
            pstmt->setString(1, login_id);
            unique_ptr<ResultSet> res{ pstmt->executeQuery() };

            if (res->next()) {
                // ȸ�����̺� login_pw �÷��� ������
                string chat_password = res->getString("login_pw");
                // �Է��� chat_password�� db���� ������ login_pw�� ��ġ�ϴ��� ��(true/false ��ȯ)
                return login_pw == chat_password;
            }
        }
        catch (const SQLException& e) {
            cout << "login failed" << e.what() << endl;
        }
        // ������ true ��ȯ �������� false ��ȯ
        return false;
    }

    // �α��� ó�� �Լ�
    void handle_login(const httplib::Request& req, httplib::Response& res) {
        try {
            json req_json = json::parse(req.body);

            string login_id = req_json["login_id"];
            string login_pw = req_json["login_pw"];

            if (check_data(login_id, login_pw)) {
                cout << u8"ȸ������ ����" << endl;
                // ���̵� ��й�ȣ ��ġ -> �α��� ���� �� jwt ��ū ��ȯ
                string token = create_jwt(login_id);

                // �α��� �����ϸ� Ŭ���̾�Ʈ�� jwt ��ū ��ȯ
                json response = { {"message", "login success"}, {"token", token} };
                // ���� ��ȯ
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

