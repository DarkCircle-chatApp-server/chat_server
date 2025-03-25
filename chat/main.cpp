#include "httplib.h"
#include <iostream>

// ä�� ���� �Լ�
void handleChat(const httplib::Request& req, httplib::Response& res) {
    
    res.set_content("chat", "text/plain");
}

int main() {
    httplib::Server svr;    // httplib::Server ��ü ����

    svr.Get("/chat", handleChat);

    // CORS ����
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // ��� �����ο��� ���� ���
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << "Chat Service ���� ��: http://localhost:5003" << std::endl;
    svr.listen("0.0.0.0", 5003); // ���� ����

    // return 0; �ϸ� �� ��, ������ ����� ������ ��� ����Ǿ�� ��
}
