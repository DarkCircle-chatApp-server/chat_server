#pragma once
#include <iostream>
#include <string>
#include <windows.h> // 한글깨짐 방지
#include "DB.hpp"  // MySQLConnector 클래스를 포함하는 헤더
#include "httplib.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class Message {
private:
    Connection* conn; // MySQL 연결 객체

public:
    Message(Connection* _conn) : conn(_conn) {
        if (!conn) {
            cerr << "Error: Connection pointer is null!" << endl;
        }
    }
    ~Message() {}
    json getMessagesJson() {
        json result_json = json::array();

        try {
            string query = R"(
                SELECT m.msg_id, u.user_name, m.msg_text, m.msg_time 
                FROM Message m 
                JOIN User u ON m.user_id = u.user_id
                ORDER BY m.msg_id DESC 
                LIMIT 15
            )";

            unique_ptr<Statement> stmt(conn->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery(query));

            while (res->next()) {
                json msg;
                msg["msg_id"] = res->getInt("msg_id");
                msg["user_name"] = res->getString("user_name");
                msg["msg_text"] = res->getString("msg_text");
                msg["msg_time"] = res->getString("msg_time");

                result_json.push_back(msg);
            }
        }
        catch (const SQLException& e) {
            cout << "Query failed: " << e.what() << endl;
        }

        return result_json;
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
    // GET 요청을 처리하여 JSON 응답 반환
    void handleMessages(const httplib::Request& req, httplib::Response& res) {
        try {
            json messages = getMessagesJson();  // 메시지 데이터 조회
            res.set_content(messages.dump(), "application/json");  // JSON 응답 반환
        }
        catch (const SQLException& e) {
            cout << "Query failed: " << e.what() << endl;
        }
    }

    //여가 15개 출력하는거 

    void print_Message() {
        int totalMessages = countMessages(); // 총 메시지 개수 가져오기

        if (totalMessages == 0) {
            cout << "No messages found in the database." << endl;
            return;
        }

        try {
            string query = R"(
            SELECT m.msg_id, u.user_name, m.msg_text, m.msg_time 
            FROM Message m 
            JOIN User u ON m.user_id = u.user_id 
        )";
            unique_ptr<Statement> stmt(conn->createStatement());
            unique_ptr<ResultSet> res(stmt->executeQuery(query));

            SetConsoleOutputCP(CP_UTF8);

            vector<pair<int, string>> messages;

            while (res->next()) {
                int msg_id = res->getInt("msg_id");
                string user_name = res->getString("user_name");
                string msg_text = res->getString("msg_text");
                string msg_time = res->getString("msg_time");

                string fullMessage = " | user_name: " + user_name +
                    " | msg_text: " + msg_text +
                    " | msg_time: " + msg_time;

                messages.emplace_back(msg_id, fullMessage);
            }

            // 최신 메시지 15개 내림차순 정렬
            sort(messages.begin(), messages.end(), [](const pair<int, string>& a, const pair<int, string>& b) {
                return a.first > b.first; // msg_id가 큰 순서대로 정렬 (내림차순)
                });

            cout << "Total Messages: " << totalMessages << endl;

            // 첫 15개 내림차순 출력 (msg_id 제외)
            int firstBatch = min(15, static_cast<int>(messages.size()));
            for (int i = 0; i < firstBatch; ++i) {
                cout << "Message Info:" << messages[i].second << endl;
            }

            if (messages.size() > 15) {
                cout << "---------------------- Remaining messages in ascending order ----------------------" << endl;

                // 남은 메시지 오름차순 정렬
                sort(messages.begin() + 15, messages.end(), [](const pair<int, string>& a, const pair<int, string>& b) {
                    return a.first < b.first; // msg_id가 작은 순서대로 정렬 (오름차순)
                    });

                // 15개씩 출력 (msg_id 제외)
                int offset = 15;
                while (offset < messages.size()) {
                    int limit = min(15, static_cast<int>(messages.size()) - offset);

                    for (int i = 0; i < limit; ++i) {
                        cout << "Message Info:" << messages[offset + i].second << endl;
                    }

                    offset += 15;

                    if (offset < messages.size()) {
                        cout << "---------------------- Next 15 messages ----------------------" << endl;
                    }
                }
            }
        }
        catch (SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }


};
