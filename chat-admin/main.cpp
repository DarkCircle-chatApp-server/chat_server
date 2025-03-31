#include "httplib.h"
#include <iostream>
#include"DB_admin.hpp"
#include "chat_ban.hpp"


#include "admin_select_delete.hpp"

// ä�� ������ ��� �Լ�
//void handle_chat_admin(const httplib::Request& req, httplib::Response& res) {
//
//    res.set_content("chat admin", "text/plain"); // ���� ����
//}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    
    httplib::Server svr;    // httplib::Server ��ü ����
    Select_delete select_delete(db.getConnection());  // getConnection()���� ��ȯ�� MySQLConnector�� conn�� signin��ü�� ����
    select_delete.All_Select();  // user ���̺� ��ȸ
    string user_id;
    cout << u8"ȸ�� ������ user_id: ";
    cin >> user_id;
    select_delete.Update_Status(user_id); //  user_id�� user_status�� 2�� ����
    //select_delete.Update_Status2(user_id); //  2���� �ڵ����� ���� ���� -> ������ ȸ��Ż��� ���Ͱ��ɱⰣ ���� ����

    string user_id2;
    cout << u8"�޼��� ������ user_id: ";
    cin >> user_id2;
    select_delete.Delete_Message(user_id2);    // �޼��� ����

    User_ban user_ban(db.getConnection());
    // �� ó��
    svr.Put("/chat/admin/ban", [&](const httplib::Request& req, httplib::Response& res) {
        user_ban.handle_user_ban(req, res);
        });

    // ������ ó��
    User_ban user_unban(db.getConnection());

    svr.Put("/chat/admin/unban", [&](const httplib::Request& req, httplib::Response& res) {
        user_unban.handle_user_unban(req, res);
        });

    // CORS ����
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // ��� �����ο��� ���� ���
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << "Chat Service running: http://localhost:5004" << std::endl;
    svr.listen("0.0.0.0", 5004); // ���� ����
    

    // return 0; �ϸ� �� ��, ������ ����� ������ ��� ����Ǿ�� ��
}
