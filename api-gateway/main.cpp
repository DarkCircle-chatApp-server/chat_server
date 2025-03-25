#include "httplib.h"
#include <iostream>

// �� �ڵ鷯 �Լ��� ����

// �⺻ API Gateway ��������Ʈ
void handleRoot(const httplib::Request&, httplib::Response& res) {
    res.set_content("API Gateway is running", "text/plain");
}

// test1 �� 5001�� ��Ʈ�� login ȣ��
void handleTest1(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5001");
    auto response = cli.Get("/login");
    if (response) {
        res.set_content(response->body, "text/plain");
    }
    else {
        res.status = 500; // ���� ���� ����
        res.set_content("Error fetching data from /login", "text/plain");
    }
}

// test2 �� 5002�� ��Ʈ�� signIn ȣ��
void handleTest2(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5002");
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

// test4 �� 5003�� ��Ʈ�� chat admin ȣ��
void handleTest4(const httplib::Request&, httplib::Response& res) {
    httplib::Client cli("http://localhost:5003");
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

    // "/" ��ο� ���� handleRoot �Լ� ����
    svr.Get("/", handleRoot);

    // "/test1" ��ο� ���� handleTest1 �Լ� ����
    svr.Get("/test1", handleTest1);

    // "/test2" ��ο� ���� handleTest2 �Լ� ����
    svr.Get("/test2", handleTest2);

    // "/test3" ��ο� ���� handleTest3 �Լ� ����
    svr.Get("/test3", handleTest3);

    // "/test4" ��ο� ���� handleTest4 �Լ� ����
    svr.Get("/test4", handleTest4);

    // CORS ����
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // ��� �����ο��� ���� ���
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    std::cout << "API Gateway ���� ��: http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    // ������ ����� ���·� ��� ����
}
