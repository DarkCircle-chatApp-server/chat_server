#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <windows.h> // 한글깨짐 방지
#include "DB.hpp"  // MySQLConnector 클래스를 포함하는 헤더
#include "httplib.h"
#include "json.hpp"
#include <sstream>

using namespace std;
using json = nlohmann::json;

class Message {
private:
    unique_ptr< sql::Connection> conn; // MySQL 연결 객체
    int hand_page = 0;

public:
    Message(unique_ptr<sql::Connection> _conn) : conn(move(_conn)) {
        if (!conn) {
            cerr << "Error: Connection pointer is null!" << endl;
        }
    }
    ~Message() {}

    // GET 요청을 처리하여 JSON 응답 반환
    void handleMessages(const httplib::Request& req, httplib::Response& res) {
    try {
        if (req.has_param("page")) {
            hand_page = stoi(req.get_param_value("page"));
        }

        json messages = print_MessageJson(hand_page);
        res.set_content(messages.dump(), "application/json");
    }
    catch (const exception& e) {
        cout << "Query failed: " << e.what() << endl;
    }
}
    // 메시지 개수 반환
    int countMessages() {
        int count = 0;
        try {
            string query = "SELECT COUNT(msg_id) AS total FROM Message";
            unique_ptr<Statement> stmt(conn->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery(query));

            if (res->next()) {
                count = res->getInt("total");
            }
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
        return count;
    }

    //여가 15개 출력하는거 

    json print_MessageJson(int page = 0) {
        json result_json = json::array();
        int totalMessages = countMessages();
        const int pageSize = 15;

        if (totalMessages == 0) {
            cout << "No messages found in the database." << endl;
            return result_json;
        }

        int offset = max(0, totalMessages - (page + 1) * pageSize); // 역순 오프셋
        int limit = min(pageSize, totalMessages - page * pageSize);

        try {
            stringstream ss;
            ss << R"(
            SELECT m.msg_id, m.user_id, u.user_name, m.msg_text, m.msg_time
            FROM Message m
            JOIN User u ON m.user_id = u.user_id
            ORDER BY m.msg_id ASC
            LIMIT )" << limit << " OFFSET " << offset;

            unique_ptr<Statement> stmt(conn->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery(ss.str()));

            while (res->next()) {
                json msg;
                msg["user_id"] = res->getInt("user_id");
                msg["user_name"] = res->getString("user_name");
                msg["msg_text"] = res->getString("msg_text");
                msg["msg_time"] = res->getString("msg_time");

                result_json.push_back(msg);
            }

            hand_page++;
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }

        return result_json;
    }


};