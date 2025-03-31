#pragma once
#include <iostream>
#include <string>
#include <windows.h> // �ѱ۱��� ����
#include "chatDB.hpp"  // MySQLConnector Ŭ������ �����ϴ� ���
#include "httplib.h"
#include "json.hpp"


using namespace std;

class Message {
private:
    Connection* conn; // MySQL ���� ��ü

public:
    Message(Connection* _conn) : conn(_conn) {
        if (!conn) {
            cerr << "Error: Connection pointer is null!" << endl;
        }
    }
    ~Message() {}
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

    void print_Message() {
        int totalMessages = countMessages(); // �� �޽��� ���� ��������

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

            // �ֽ� �޽��� 15�� �������� ����
            sort(messages.begin(), messages.end(), [](const pair<int, string>& a, const pair<int, string>& b) {
                return a.first > b.first; // msg_id�� ū ������� ���� (��������)
                });

            cout << "Total Messages: " << totalMessages << endl;

            // ù 15�� �������� ��� (msg_id ����)
            int firstBatch = min(15, static_cast<int>(messages.size()));
            for (int i = 0; i < firstBatch; ++i) {
                cout << "Message Info:" << messages[i].second << endl;
            }

            if (messages.size() > 15) {
                cout << "---------------------- Remaining messages in ascending order ----------------------" << endl;

                // ���� �޽��� �������� ����
                sort(messages.begin() + 15, messages.end(), [](const pair<int, string>& a, const pair<int, string>& b) {
                    return a.first < b.first; // msg_id�� ���� ������� ���� (��������)
                    });

                // 15���� ��� (msg_id ����)
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
