#define NDEBUG
#include "httplib.h"    // httplib ������� �߰�
#include <iostream>
#include "DBmodule.hpp"
#include "signinClass.hpp"
#include <Windows.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
//#include <nlohmann/json.hpp>  // JSON �Ľ��� ���� ���̺귯��

using namespace std;


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

int main() {
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_library_init();
    SetConsoleOutputCP(CP_UTF8);    // �ܼ� ��� ���ڵ�. �ѱ��Է°� ���ʿ� u8 �ٿ��� ��
    MySQLConnector db(SERVER_IP, USERNAME, PASSWORD, DATABASE);
	auto connection = db.getConnection();

	SignIn signin(connection);  // getConnection()���� ��ȯ�� MySQLConnector�� conn�� signin��ü�� ����;
    //SignIn signin(db.getConnection());  // getConnection()���� ��ȯ�� MySQLConnector�� conn�� signin��ü�� ����
    cout << "test start" << endl;
    //signin.get_user_status("admin12345");

    httplib::Server svr;    // httplib::Server ��ü ����

    /*svr.Options("/login", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
    });
    svr.Options("/signIn", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });*/

    // "/login" URL�� ������ GET ��û�� handleLogin �Լ��� ó��
    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        signin.handle_login(req, res);
    });
    
    svr.Post("/idCheck", [&](const httplib::Request& req, httplib::Response& res) {
        signin.check_validation(req, res);
    });

    svr.Get("/statCheck/:login_id", [&](const httplib::Request& req, httplib::Response& res) {
        signin.check_user_status(req, res);
    });

    svr.Get("/getName/:login_id", [&](const httplib::Request& req, httplib::Response& res) {
        signin.show_name(req, res);
    });

    svr.Get("/showId/:login_id", [&](const httplib::Request& req, httplib::Response& res) {
        signin.show_id(req, res);
    });

    // ���̵� �ߺ� üũ
    /*svr.Post("/idCheck", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "Request received at /idCheck" << endl;
        try {
            if (!signin.check_validation(req, res)) {
                cout << "ID already exists, 400 response sent" << endl;
                return;
            }
            res.status = 200;
            res.set_content("ID is available", "text/plain");
            cout << "ID is available, 200 response sent" << endl;
        }
        catch (const std::exception& e) {
            res.status = 500;
            res.set_content("Internal Server Error: " + string(e.what()), "text/plain");
            cerr << "Error: " << e.what() << endl;
        }
    });*/


    // "/signIn" URL�� ������ POST ��û�� handleSignIn �Լ��� ó��
    // ���ٽ� �� ���� : ���� �Ⱦ��� signin.handleSignIn() �̷������� �ܺ� ��ü �Լ��� ���� ���Ѵٰ� ��..�ù�
    // [&] : ĸó ����Ʈ - ���� �������� �������� ���� ������� ĸó(���� �̷��� �����ִµ� �׳� ���ٽ� �տ� ���̴� �ڵ��ε���)
    svr.Post("/signIn", [&](const httplib::Request& req, httplib::Response& res) {
        //std::cout << "Request body: " << req.body << std::endl;
        signin.handle_signIn(req, res);
    });

    // ȸ�� Ż�� ��û
    svr.Put("/update2", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "call success" << endl;
        signin.handle_delete(req, res);
    });

    // CORS ����(8080 ��Ʈ���� ������ ��û ���)
    svr.set_default_headers({
        { "Access-Control-Allow-Origin", "*" },     // ��� �����ο��� ���� ���
        { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
        { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
        });

    // 5001�� ��Ʈ�� ������ Ŭ���̾�Ʈ ��û ����
    std::cout << "Login Service running: http://localhost:8880" << std::endl;
    std::cout << "SignIn Service running: http://localhost:8880" << std::endl;
    svr.listen("0.0.0.0", 8880);

    //svr.listen("0.0.0.0", 5002);

    // return 0; �ϸ� �� �� return ����Ǹ� ���α׷� �����. ���� ����� ä�� ��� ����. ����� �츮�� �ʿ��� �� �ؾ���
}

