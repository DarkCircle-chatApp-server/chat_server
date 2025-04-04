#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <windows.h> // �ѱ۱��� ����
#include "DB.hpp"  // MySQLConnector Ŭ������ �����ϴ� ���
#include "httplib.h"
#include "json.hpp"
#include <sstream>

using namespace std;
using json = nlohmann::json;

class Message {
private:
    unique_ptr< sql::Connection> conn; // MySQL ���� ��ü
    int hand_page = 0;

public:
    Message(unique_ptr<sql::Connection> _conn) : conn(move(_conn)) {
        if (!conn) {
            cerr << "Error: Connection pointer is null!" << endl;
        }
    }
    ~Message() {}

    // GET ��û�� ó���Ͽ� JSON ���� ��ȯ
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
    // �޽��� ���� ��ȯ
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

    //���� 15�� ����ϴ°� 

    json print_MessageJson(int page = 0) {
        json result_json = json::array();
        int totalMessages = countMessages();
        const int pageSize = 15;

        if (totalMessages == 0) {
            cout << "No messages found in the database." << endl;
            return result_json;
        }

        int offset = max(0, totalMessages - (page + 1) * pageSize); // ���� ������
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