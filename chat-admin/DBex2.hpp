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
            // 현재 user_status 확인
            unique_ptr<PreparedStatement> checkStmt(conn->prepareStatement("SELECT user_status FROM User WHERE user_id = ?"));
            checkStmt->setString(1, user_id);
            unique_ptr<ResultSet> res(checkStmt->executeQuery());

            if (res->next()) {
                int current_status = res->getInt("user_status");
                if (current_status == 1) {
                    cout << u8"이미 삭제된 user_id: " << user_id << endl;
                    return;
                }

                else {
                    // user_status 변경 (회원 삭제 처리)
                    unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE User SET user_status = 1 WHERE user_id = ?"));
                    pstmt->setString(1, user_id);   // 첫번째 물음표 지정
                    int Status_Change = pstmt->executeUpdate();

                    if (Status_Change > 0) { // 삭제할 행이 있으면 실행
                        cout << "User ID " << user_id << u8" 상태가 1로 변경되었습니다." << endl;
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
                cout <<u8"삭제할 메시지가 없습니다." << endl;
            }
        }
        catch (SQLException& e) {
            cout << u8"실패: " << e.what() << endl;
        }
    }

    ~MySQLConnector() {
        cout << "MySQL Disconnect!!" << endl;
    }
};

//int main() {
//    SetConsoleOutputCP(CP_UTF8);
//    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
//    db.All_Select();  // user 테이블 조회
//    
//    string user_id;    
//    cout << u8"회원 삭제할 user_id: ";
//    cin >> user_id;
//    db.Update_Status(user_id); //  user_id의 user_status를 1로 변경
//
//    string user_id2;
//    cout << u8"메세지 삭제할 user_id: ";
//    cin >> user_id2;
//    db.Delete_Message(user_id2);    // 메세지 삭제
//
//    return 0;
//}
