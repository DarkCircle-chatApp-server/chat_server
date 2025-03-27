#include<iostream>
#include<mysql/jdbc.h>
#include<windows.h>
#include "DBex2.hpp"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    db.All_Select();  // user 테이블 조회
    
    string user_id;    
    cout << u8"회원 삭제할 user_id: ";
    cin >> user_id;
    db.Update_Status(user_id); //  user_id의 user_status를 1로 변경

    string user_id2;
    cout << u8"메세지 삭제할 user_id: ";
    cin >> user_id2;
    db.Delete_Message(user_id2);    // 메세지 삭제

    return 0;
}
