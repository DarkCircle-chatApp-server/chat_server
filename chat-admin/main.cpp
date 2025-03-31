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
    
    Select_delete select(db.getConnection());   // ��ȸ
    svr.Put("/chat/admin/user_select", [&](const httplib::Request& req, httplib::Response& res) {
        select.handle_admin_select(req, res);
        });

    Select_delete user_delete(db.getConnection());  // ���� ����
    svr.Put("/chat/admin/user_delete", [&](const httplib::Request& req, httplib::Response& res) {
        user_delete.handle_admin_select(req, res);
        });

    Select_delete message_delete(db.getConnection());   // �޼��� ����
    svr.Put("/chat/admin/user_delete", [&](const httplib::Request& req, httplib::Response& res) {
        message_delete.handle_amdim_message_delete(req, res);
        });


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
