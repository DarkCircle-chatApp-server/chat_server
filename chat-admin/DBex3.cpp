#include<iostream>
#include<mysql/jdbc.h>
#include<windows.h>
#include "DBex2.hpp"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    db.All_Select();  // user ���̺� ��ȸ
    
    string user_id;    
    cout << u8"ȸ�� ������ user_id: ";
    cin >> user_id;
    db.Update_Status(user_id); //  user_id�� user_status�� 1�� ����

    string user_id2;
    cout << u8"�޼��� ������ user_id: ";
    cin >> user_id2;
    db.Delete_Message(user_id2);    // �޼��� ����

    return 0;
}
