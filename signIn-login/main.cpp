#include "httplib.h"    // httplib ������� �߰�
#include <iostream>
#include "DBmodule.hpp"
#include "signinClass.hpp"
#include <Windows.h>
//#include <nlohmann/json.hpp>  // JSON �Ľ��� ���� ���̺귯��


// �ʿ��� �Լ����� ������Ͽ� �����
// �ϴ� ���δ� ������ GET ��û���θ� api�� ��������. ��ɿ� ���� GET, POST, PUT, DELETE �� ������ �޼���� ������ ��
// rest api�� url�� ���Ե� ��ҵ�� �����Ϳ� �����ؾ���
// �޽��� ���� api ����) /chat/1(userid��. �������� �ҷ��;���)/sendMessage
// �̷������� url�� ��ҵ��� �־ �����Ϸ��� �ν��Ͽ� ó���ϵ��� ��������
// �� svr.POST("/chat/{userid}/sendMessage", [](const httplib::Request& req, httplib::Response& res)) �̷������� �޽��� ���� api�� ����°���
// userid�� mysql db ������ �Ǵ� �α��μ��ǿ��� �����;���
// ���� ���� �� �� ��. �ٵ� ������ ���۸��غ��ô�...!

// �ϴ� ���� ���ǻ� main.cpp�� �Լ��� ���� �س���. ���߿� ������Ϸ� �Űܾ���
// ������� ���ô� ���� �� ������Ʈ ���� test1.hpp, test2.hpp ������� Ȯ���� ��
// �α��� ó�� �Լ�
void handleLogin(const httplib::Request& req, httplib::Response& res) {     // Request& req: Ŭ���̾�Ʈ�� ��û, 
                                                                            // httplib::Response& res: ���� ������ ���� ��ü
    // ���⿡ db�����ؼ� ������ ó���ϴ°Ŷ�
    // redis�� ��û������ �ڵ� �־����

    // ó���� ��� ���� ��ȯ
    res.set_content("login success", "text/plain");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);    // �ܼ� ��� ���ڵ�. �ѱ��Է°� ���ʿ� u8 �ٿ��� ��
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
    SignIn signin(db.getConnection());  // getConnection()���� ��ȯ�� MySQLConnector�� conn�� signin��ü�� ����

    httplib::Server svr;    // httplib::Server ��ü ����

    // "/login" URL�� ������ GET ��û�� handleLogin �Լ��� ó��
    svr.Get("/login", handleLogin);

    // "/signIn" URL�� ������ POST ��û�� handleSignIn �Լ��� ó��
    // ���ٽ� �� ���� : ���� �Ⱦ��� signin.handleSignIn() �̷������� �ܺ� ��ü �Լ��� ���� ���Ѵٰ� ��..�ù�
    // [&] : ĸó ����Ʈ - ���� �������� �������� ���� ������� ĸó(���� �̷��� �����ִµ� �׳� ���ٽ� �տ� ���̴� �ڵ��ε���)
    svr.Post("/signIn", [&](const httplib::Request& req, httplib::Response& res) {
        signin.handleSignIn(req, res);
    });

    // CORS ����(�ٸ� ��Ʈ��ȣ����(react ��Ʈ:3000) ������ ��û ���)
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // ��� �����ο��� ���� ���
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    // 5001�� ��Ʈ�� ������ Ŭ���̾�Ʈ ��û ����
    std::cout << "Login Service running: http://localhost:5001" << std::endl;
    std::cout << "SignIn Service running: http://localhost:5001" << std::endl;
    svr.listen("0.0.0.0", 5001);

    //svr.listen("0.0.0.0", 5002);

    // return 0; �ϸ� �� �� return ����Ǹ� ���α׷� �����. ���� ����� ä�� ��� ����. ����� �츮�� �ʿ��� �� �ؾ���
}

