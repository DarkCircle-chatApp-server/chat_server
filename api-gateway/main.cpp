#include "httplib.h"
#include <iostream>

// 각 핸들러 함수 정의

// 기본 API Gateway 엔드포인트
void handleRoot(const httplib::Request&, httplib::Response& res) {
    res.set_content("API Gateway is running", "text/plain");
}

// test1 → 5001번 포트의 login 호출
void routing_login(const httplib::Request& req, httplib::Response& res) {
    /*res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");*/
    httplib::Client cli("http://localhost:8880");
    auto response = cli.Post("/login", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /login", "text/plain");
    }
}

// test2 → 5001번 포트의 signIn 호출
void routing_signup(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8880");
    auto response = cli.Post("/signIn", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = response->status;
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /signIn", "text/plain");
    }
}

// test3 → 5001번 포트의 idCheck 호출
void routing_check(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8880");
    auto response = cli.Post("/idCheck", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = response->status;
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /signIn", "text/plain");
    }
}

// 5001번 포트의 /statCheck/:user_id 호출
void routing_statCheck(const httplib::Request& req, httplib::Response& res) {
    if (req.path_params.find("login_id") == req.path_params.end()) {
        res.status = 400;
        res.set_content("login_id가 없습니다.", "text/plain");
        return;
    }
    std::string login_id = req.path_params.at("login_id");
    httplib::Client cli("http://localhost:8880");
    std::string url = "/statCheck/" + login_id;
    auto response = cli.Get(url.c_str());
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(response->body, "application/json");
    }
    else {
        res.status = 500;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /statCheck", "text/plain");
    }
}

void routing_getName(const httplib::Request& req, httplib::Response& res) {
    if (req.path_params.find("login_id") == req.path_params.end()) {
        res.status = 400;
        res.set_content("login_id가 없습니다.", "text/plain");
        return;
    }
    std::string login_id = req.path_params.at("login_id");
    httplib::Client cli("http://localhost:8880");
    std::string url = "/getName/" + login_id;
    auto response = cli.Get(url.c_str());
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(response->body, "application/json");
    }
    else {
        res.status = 500;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /statCheck", "text/plain");
    }
}

void routing_showId(const httplib::Request& req, httplib::Response& res) {
    if (req.path_params.find("login_id") == req.path_params.end()) {
        res.status = 400;
        res.set_content("login_id가 없습니다.", "text/plain");
        return;
    }
    std::string login_id = req.path_params.at("login_id");
    httplib::Client cli("http://localhost:8880");
    std::string url = "/showId/" + login_id;
    auto response = cli.Get(url.c_str());
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(response->body, "application/json");
    }
    else {
        res.status = 500;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /statCheck", "text/plain");
    }
}

void routing_ban(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8882");
    auto response = cli.Put("/chat/admin/ban", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = response->status;
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /signIn", "text/plain");
    }
}

void routing_showUsers(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8882");
    std::string url = "/chat/admin/admin_select";
    auto response = cli.Get(url.c_str());
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(response->body, "application/json");
    }
    else {
        res.status = 500;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /chat/admin/user_select", "text/plain");
    }
}

void routing_adminBan(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8882");
    auto response = cli.Put("/chat/admin/user_delete", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = response->status;
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /chat/admin/user_delete", "text/plain");
    }
}

void routing_adminUnban(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8882");
    auto response = cli.Put("/chat/admin/unban", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = response->status;
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /chat/admin/unban", "text/plain");
    }
}

void routing_setAdmin(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8882");
    auto response = cli.Put("/chat/admin/status_update", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = response->status;
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /chat/admin/unban", "text/plain");
    }
}

void routing_admin_select(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8882");
    auto response = cli.Post("/chat/admin/user_select", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = response->status;
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /signIn", "text/plain");
    }
}

// 5003포트 채팅 전송
void routing_send_chat(const httplib::Request& req, httplib::Response& res) {
    /*res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");*/
    httplib::Client cli("http://localhost:8881");
    auto response = cli.Post("/chat/room", req.body, "application/json");
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.status = response->status;
        std::cout << "Backend response: " << response->body << std::endl;
        res.set_content(response->body, response->get_header_value("Content-Type"));
    }
    else {
        res.status = 500; // 서버 오류 응답
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /login", "text/plain");
    }
}

void routing_showMessages(const httplib::Request& req, httplib::Response& res) {
    httplib::Client cli("http://localhost:8881");
    std::string url = "/chat/messages";
    auto response = cli.Get(url.c_str());
    if (response) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(response->body, "application/json");
    }
    else {
        res.status = 500;
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content("Error fetching data from /chat/admin/user_select", "text/plain");
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

    svr.Options("/signIn", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/idCheck", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/statCheck/:login_id", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
    });

    svr.Options("/getName/:login_id", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/showId/:login_id", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/admin/ban", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/admin/user_select", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/admin/user_delete", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/admin/unban", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/admin/status_update", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/admin/user_select", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/room", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    svr.Options("/chat/messages", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 204;
        });

    // "/" 경로에 대해 handleRoot 함수 연결
    svr.Get("/", handleRoot);

    // "/test1" 경로에 대해 handleTest1 함수 연결
    svr.Post("/login", routing_login);

    // "/test2" 경로에 대해 handleTest2 함수 연결
    svr.Post("/signIn", routing_signup);

    // "/test3" 경로에 대해 handleTest3 함수 연결
    svr.Post("/idCheck", routing_check);

    // "/test4" 경로에 대해 handleTest4 함수 연결
    svr.Get("/statCheck/:login_id", routing_statCheck);

    svr.Get("/getName/:login_id", routing_getName);

    svr.Get("/showId/:login_id", routing_showId);

    svr.Put("/chat/admin/ban", routing_ban);

    svr.Get("/chat/admin/admin_select", routing_showUsers);

    svr.Put("/chat/admin/user_delete", routing_adminBan);

    svr.Put("/chat/admin/unban", routing_adminUnban);

    svr.Put("/chat/admin/status_update", routing_setAdmin);

    svr.Post("/chat/admin/user_select", routing_admin_select);

    svr.Post("/chat/room", routing_send_chat);
    
    svr.Get("/chat/messages", routing_showMessages);


    // CORS 설정
    //svr.set_default_headers({
    //    { "Access-Control-Allow-Origin", "*" },     // 모든 도메인에서 접근 허용
    //    { "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE" },
    //    { "Access-Control-Allow-Headers", "Content-Type, Authorization" }
    //    });

    std::cout << "API Gateway 실행 중: http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
}
