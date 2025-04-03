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
    shared_ptr< sql::Connection> conn; // MySQL ���� ��ü

public:
    Message(shared_ptr<sql::Connection> _conn) : conn(move(_conn)) {
        if (!conn) {
            cerr << "Error: Connection pointer is null!" << endl;
        }
    }
    ~Message() {}

    // GET ��û�� ó���Ͽ� JSON ���� ��ȯ
    void handleMessages(const httplib::Request& req, httplib::Response& res) {
        try {
            json messages = print_MessageJson();  // �޽��� ������ ��ȸ
            res.set_content(messages.dump(), "application/json");  // JSON ���� ��ȯ
        }
        catch (const SQLException& e) {
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

    json print_MessageJson() {
        json result_json = json::array(); // JSON �迭 ����
        int totalMessages = countMessages(); // �� �޽��� ���� ��������

        if (totalMessages == 0) {
            cout << "No messages found in the database." << endl;
            return result_json;
        }
        // SQL�� R�� �����Ͽ� �����͸� ó���ϸ� �ؽ�Ʈ.��¥/�ð�, ���� ���� ���Ե� �Ϻ� ������ ó���� �����ؼ� ���� ���� �ص� ������ ����
        try {                   
            string query = R"(                                                      
            SELECT m.msg_id, m.user_id, u.user_name, m.msg_text, m.msg_time 
            FROM Message m 
            JOIN User u ON m.user_id = u.user_id 
        )";

            unique_ptr<Statement> stmt(conn->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery(query));

            vector<json> messages;

            while (res->next()) {
                json msg;
                msg["msg_id"] = res->getInt("msg_id");  // msg_id ���� (���Ŀ�)
                msg["user_id"] = res->getInt("user_id");
                msg["user_name"] = res->getString("user_name");
                msg["msg_text"] = res->getString("msg_text");
                msg["msg_time"] = res->getString("msg_time");

                messages.push_back(msg);
            }

            // �ֽ� �޽��� 15�� �������� ����
            sort(messages.begin(), messages.end(), [](const json& a, const json& b) {
                return a["msg_id"] > b["msg_id"]; // msg_id�� ū ������� ���� (��������)
                });

            // �ֽ� 15�� �޽��� �߰� (msg_id ���� �� ����)
            int firstBatch = min(15, static_cast<int>(messages.size()));
            for (int i = 0; i < firstBatch; ++i) {
                messages[i].erase("msg_id");  // msg_id ����
                json ordered_msg;  // ������ ���� ���ο� JSON ��ü
                ordered_msg["user_id"] = messages[i]["user_id"];
                ordered_msg["user_name"] = messages[i]["user_name"];
                ordered_msg["msg_text"] = messages[i]["msg_text"];
                ordered_msg["msg_time"] = messages[i]["msg_time"];

                result_json.push_back(messages[i]);
            }

            // ���� �޽��� �������� ����
            if (messages.size() > 15) {
                sort(messages.begin() + 15, messages.end(), [](const json& a, const json& b) {
                    return a["msg_id"] < b["msg_id"]; // msg_id�� ���� ������� ���� (��������)
                    });

                // ���� �޽��� �߰� (msg_id ���� �� ����)
                for (size_t i = 15; i < messages.size(); ++i) {
                    messages[i].erase("msg_id");  // msg_id ����
                    json ordered_msg;
                    ordered_msg["user_id"] = messages[i]["user_id"];
                    ordered_msg["user_name"] = messages[i]["user_name"];
                    ordered_msg["msg_text"] = messages[i]["msg_text"];
                    ordered_msg["msg_time"] = messages[i]["msg_time"];

                    result_json.push_back(messages[i]);
                }
            }
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }

        return result_json;
    }


};
