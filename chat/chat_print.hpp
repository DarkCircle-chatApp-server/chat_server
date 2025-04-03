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
    shared_ptr< sql::Connection> conn; // MySQL 연결 객체

public:
    Message(shared_ptr<sql::Connection> _conn) : conn(move(_conn)) {
        if (!conn) {
            cerr << "Error: Connection pointer is null!" << endl;
        }
    }
    ~Message() {}

    // GET 요청을 처리하여 JSON 응답 반환
    void handleMessages(const httplib::Request& req, httplib::Response& res) {
        try {
            json messages = print_MessageJson();  // 메시지 데이터 조회
            res.set_content(messages.dump(), "application/json");  // JSON 응답 반환
        }
        catch (const SQLException& e) {
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

    json print_MessageJson() {
        json result_json = json::array(); // JSON 배열 생성
        int totalMessages = countMessages(); // 총 메시지 개수 가져오기

        if (totalMessages == 0) {
            cout << "No messages found in the database." << endl;
            return result_json;
        }
        // SQL과 R을 연계하여 데이터를 처리하면 텍스트.날짜/시간, 숫자 값이 포함된 일부 데이터 처리가 가능해서 넣음 빼고 해도 문제는 없음
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
                msg["msg_id"] = res->getInt("msg_id");  // msg_id 포함 (정렬용)
                msg["user_id"] = res->getInt("user_id");
                msg["user_name"] = res->getString("user_name");
                msg["msg_text"] = res->getString("msg_text");
                msg["msg_time"] = res->getString("msg_time");

                messages.push_back(msg);
            }

            // 최신 메시지 15개 내림차순 정렬
            sort(messages.begin(), messages.end(), [](const json& a, const json& b) {
                return a["msg_id"] > b["msg_id"]; // msg_id가 큰 순서대로 정렬 (내림차순)
                });

            // 최신 15개 메시지 추가 (msg_id 제거 후 저장)
            int firstBatch = min(15, static_cast<int>(messages.size()));
            for (int i = 0; i < firstBatch; ++i) {
                messages[i].erase("msg_id");  // msg_id 삭제
                json ordered_msg;  // 순서를 맞춘 새로운 JSON 객체
                ordered_msg["user_id"] = messages[i]["user_id"];
                ordered_msg["user_name"] = messages[i]["user_name"];
                ordered_msg["msg_text"] = messages[i]["msg_text"];
                ordered_msg["msg_time"] = messages[i]["msg_time"];

                result_json.push_back(messages[i]);
            }

            // 남은 메시지 오름차순 정렬
            if (messages.size() > 15) {
                sort(messages.begin() + 15, messages.end(), [](const json& a, const json& b) {
                    return a["msg_id"] < b["msg_id"]; // msg_id가 작은 순서대로 정렬 (오름차순)
                    });

                // 남은 메시지 추가 (msg_id 제거 후 저장)
                for (size_t i = 15; i < messages.size(); ++i) {
                    messages[i].erase("msg_id");  // msg_id 삭제
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
