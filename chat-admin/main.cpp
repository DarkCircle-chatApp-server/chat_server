#include "httplib.h"
#include <iostream>
#include"DB_admin.hpp"
#include "chat_ban.hpp"

// ä�� ������ ��� �Լ�
//void handle_chat_admin(const httplib::Request& req, httplib::Response& res) {
//
//    res.set_content("chat admin", "text/plain"); // ���� ����
//}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    User_ban user_ban(db.getConnection());
    
    
    httplib::Server svr;    // httplib::Server ��ü ����

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
