#pragma once

#include<iostream>
#include<mysql/jdbc.h>
#include<windows.h>


using namespace std;
using namespace sql;

#define SERVER_IP	"tcp://127.0.0.1:3306"
#define USERNAME	"root"
#define PASSWORD	"12345"
#define DATABASE	"chat"

class MySQLConnector {
private:
    string server;
    string username;
    string password;
    string database;
    unique_ptr<Connection> conn;

public:
    MySQLConnector(const string& serv, const string& user, const string& pw, const string& db)
        : server(serv), username(user), password(pw), database(db) {
        try {
            mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
            conn = unique_ptr<Connection>(driver->connect(server, username, password));
            conn->setSchema(database);
            cout << "MySQL Connection success" << endl;
        }
        catch (SQLException& e) {
            cout << "MySQL Connection failed: " << e.what() << endl;
        }
    }

    	void All_Select() {
		try {
			unique_ptr<Statement> stmt(conn->createStatement());
			unique_ptr<ResultSet> res(stmt->executeQuery("SELECT* FROM User"));
		
			cout << "user table: " << endl;
			while (res->next()) {
				cout << res->getString("user_id") << " " 
                    << res->getString("login_id") <<" "
                    << res->getString("login_pw") << " " 
                    << res->getString("user_name")<< " " 
                    << (res->isNull("user_addr") ? "null": res->getString("user_addr").c_str()) << " " << 
					(res->isNull("user_phone") ? "null" : res->getString("user_phone").c_str()) << " " 
                    << res->getString("user_status")<< " " 
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
                if (current_status == 1) {
                    cout << u8"�̹� ������ user_id: " << user_id << endl;
                    return;
                }

                else {
                    // user_status ���� (ȸ�� ���� ó��)
                    unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?"));
                    pstmt->setString(1, user_id);   // ù��° ����ǥ ����
                    int Status_Change = pstmt->executeUpdate();

                    if (Status_Change > 0) { // ������ ���� ������ ����
                        cout << "User ID " << user_id << u8" ���°� 1�� ����Ǿ����ϴ�." << endl;
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
                cout <<u8"������ �޽����� �����ϴ�." << endl;
            }
        }
        catch (SQLException& e) {
            cout << u8"����: " << e.what() << endl;
        }
    }

    ~MySQLConnector() {
        cout << "MySQL Disconnect!!" << endl;
    }
};

//int main() {
//    SetConsoleOutputCP(CP_UTF8);
//    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
//    db.All_Select();  // user ���̺� ��ȸ
//    
//    string user_id;    
//    cout << u8"ȸ�� ������ user_id: ";
//    cin >> user_id;
//    db.Update_Status(user_id); //  user_id�� user_status�� 1�� ����
//
//    string user_id2;
//    cout << u8"�޼��� ������ user_id: ";
//    cin >> user_id2;
//    db.Delete_Message(user_id2);    // �޼��� ����
//
//    return 0;
//}
