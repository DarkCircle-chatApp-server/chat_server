#include "httplib.h"
#include <iostream>

// �� �ڵ鷯 �Լ��� ����

// �⺻ API Gateway ��������Ʈ
void handleRoot(const httplib::Request&, httplib::Response& res) {
    res.set_content("API Gateway is running", "text/plain");
}

// test1 �� 5001�� ��Ʈ�� login ȣ��
void routing_login(const httplib::Request& req, httplib::Response& res) {
    /*res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");*/
    httplib::Client cli("http://localhost:5001");
    auto response = cli.Post("/login", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // ���� ���� ����
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /login", "text/plain");
    }
}

// test2 �� 5001�� ��Ʈ�� signIn ȣ��
void handleTest2(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5001");
    auto response = cli.Get("/signIn");
    if (response) {
        res.set_content(response->body, "text/plain");
    }
    else {
        res.status = 500; // ���� ���� ����
        res.set_content("Error fetching data from /signIn", "text/plain");
    }
}

// test3 �� 5003�� ��Ʈ�� chat ȣ��
void handleTest3(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5003");
    auto response = cli.Get("/chat");
    if (response) {
        res.set_content(response->body, "text/plain");
    }
    else {
        res.status = 500; // ���� ���� ����
        res.set_content("Error fetching data from /chat", "text/plain");
    }
}

// test4 �� 5004�� ��Ʈ�� chat admin ȣ��
void handleTest4(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5004");
    auto response = cli.Get("/chat/admin");
    if (response) {
        res.set_content(response->body, "text/plain");
    }
    else {
        res.status = 500; // ���� ���� ����
        res.set_content("Error fetching data from /chat/admin", "text/plain");
    }
}

int main() {
    httplib::Server svr;

    svr.Options("/login", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204; 
    });

    // "/" ��ο� ���� handleRoot �Լ� ����
    svr.Get("/", handleRoot);

    // "/test1" ��ο� ���� handleTest1 �Լ� ����
    svr.Post("/login", routing_login);

    // "/test2" ��ο� ���� handleTest2 �Լ� ����
    svr.Get("/test2", handleTest2);

    // "/test3" ��ο� ���� handleTest3 �Լ� ����
    svr.Get("/test3", handleTest3);

    // "/test4" ��ο� ���� handleTest4 �Լ� ����
    svr.Get("/test4", handleTest4);

    // CORS ����
    //svr.set_default_headers({
    //    { "Access-Control-Allow-Origin", "*" },     // ��� �����ο��� ���� ���
    //    { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
    //    { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
    //    });

    std::cout << "API Gateway ���� ��: http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
}
