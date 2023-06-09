#ifndef TICKET_SYSTEM_USER_SYSTEM_H
#define TICKET_SYSTEM_USER_SYSTEM_H

#include <cstring>
#include "MyString.h"
#include "map.h"
#include "bpt.h"

struct user_data {
    String<31> password;
    String<20> name;
    String<31> mailAddr;
    int privilege;

    user_data() {}

    user_data(const user_data &a) : password(a.password), name(a.name), mailAddr(a.mailAddr),
                                    privilege(a.privilege) {}

    friend bool operator==(const user_data &a, const user_data &b) {
        return a.password == b.password && a.name == b.name && a.mailAddr == b.mailAddr && a.privilege == b.privilege;
    }

    friend bool operator<=(const user_data &a, const user_data &b) {
        return true;
    }

    friend bool operator<(const user_data &a, const user_data &b) {
        return false;
    }

    friend bool operator>(const user_data &a, const user_data &b) {
        return false;
    }

    friend bool operator>=(const user_data &a, const user_data &b) {
        return true;
    }

    user_data &operator=(const user_data &a) {
        if (this == &a) return *this;
        password = a.password;
        name = a.name;
        mailAddr = a.mailAddr;
        privilege = a.privilege;
        return *this;
    }
};

class User_System {
private:

    bpt<String<21>, user_data> users{"user_node", "user_leaf"};

    map<String<21>, bool> user_list;


public:
    User_System() {}

    ~User_System() {};


    int add_first_user(const String<21> &username, const user_data &data) {
        users.insert(username, data);
//        user_list.insert(std::pair<String<21>,bool>(username, false));
        return 0;
    }

    int add_user(const String<21> &cur_username, const String<21> &username, const user_data &data) {
        auto flag = user_list.find(cur_username);
        auto cur = users.find(cur_username);
        auto exist = users.find(username);
        if (flag != user_list.end() && !cur.empty() && exist.empty() && data.privilege < cur.back().privilege) {
            if (users.find(username).empty()) {
                users.insert(username, data);
                return 0;
            }
        }
        return -1;
    }

    bool empty() {
        return users.empty();
    }

    bool check_login(const String<21> &username) {
        auto flag = user_list.find(username);
        if (flag == user_list.end()) return false;
        return true;
    }

    int login(const String<21> &username, const String<31> &pass) {
        auto flag = user_list.find(username);
        auto cur = users.find(username);
        if (flag == user_list.end() && !cur.empty() && cur.back().password == pass) {
            user_list.insert(std::pair<String<21>, bool>(username, true));
            return 0;
        }
        return -1;
    }

    int logout(const String<21> &username) {
        auto flag = user_list.find(username);
        if (flag != user_list.end()) {
            user_list.erase(flag);
            return 0;
        }
        return -1;
    }

    std::string query_profile(const String<21> &cur_username, const String<21> &u) {
        auto flag = user_list.find(cur_username);
        auto cur = users.find(cur_username);
        auto now = users.find(u);
        if (flag != user_list.end() && !cur.empty() && !now.empty() &&
            (cur.back().privilege > now.back().privilege || cur.back() == now.back())) {
            auto tmp = now.back();
            std::string username, name, mailAddr, pri;
            username = u.str;
            name = tmp.name.str;
            mailAddr = tmp.mailAddr.str;
            pri = std::to_string(tmp.privilege);
            username += (" " + name + " " + mailAddr + " " + pri);
            return username;
        }
        return "-1";
    }


    std::string modify_profile(const String<21> &c, const String<21> &u, vector<std::string> phrases) {
        auto flag = user_list.find(c);
        auto cur = users.find(c);
        auto now = users.find(u);
        if (flag != user_list.end() && !cur.empty() && !now.empty() &&
            (cur.back().privilege > now.back().privilege || cur.back() == now.back())) {
            user_data data = now.back();
            user_data tmp = data;
            int i = 1;
            while (i < phrases.size() - 1) {
                std::string parm = phrases[++i];
                if (parm == "-p") {
                    strcpy(data.password.str, phrases[++i].c_str());
                } else if (parm == "-n") {
                    strcpy(data.name.str, phrases[++i].c_str());
                } else if (parm == "-m") {
                    strcpy(data.mailAddr.str, phrases[++i].c_str());
                } else if (parm == "-g") {
                    int pri = atoi(phrases[++i].c_str());
                    if (pri < cur.back().privilege) {
                        data.privilege = pri;
                    } else return "-1";
                }
            }
            users.remove(u, tmp);
            users.insert(u, data);
            std::string username, name, mailAddr, pri;
            username = u.str;
            name = data.name.str;
            mailAddr = data.mailAddr.str;
            pri = std::to_string(data.privilege);
            username += (" " + name + " " + mailAddr + " " + pri);
            return username;
        }
        return "-1";
    }

    void clean() {
        users.clean();
    }

};


#endif //TICKET_SYSTEM_USER_SYSTEM_H
