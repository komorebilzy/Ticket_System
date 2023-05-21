#include <iostream>
#include "vector.h"
#include "user_system.h"
#include "train_system.h"
#include "MyString.h"
#include "Data.h"
#include <cstring>
#include <cstdio>

using namespace std;

vector<string> phrase(const string &str) {
    vector<string> phrases;
    char op;
    int i = 0;
    while (i < str.size()) {
        string element = "";
        while (!isspace(op = str[i])) {
            element += op;
            ++i;
            if(i==str.size()) break;
        }
        if(element.size())phrases.push_back(element);
        ++i;
    }
    return phrases;
}


int main() {
    freopen("data/data/basic_1/1.in","r",stdin);
    freopen("my_ans","w",stdout);
    User_System user_system;
    Train_system train_system;
    string str;
    while (!cin.eof()) {
        getline(cin, str);
        vector<string> phrases(phrase(str));
        string timeOrder = phrases[0];
        cout << timeOrder << " ";
        string order = phrases[1];
        int i = 1;
        int siz=phrases.size()-1;
        if (order == "add_user") {
            String<25> cur_username;
            String<25> username;
            user_data data;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-c") {
                    strcpy(cur_username.str, phrases[++i].c_str());
                } else if (parm == "-u") {
                    strcpy(username.str, phrases[++i].c_str());
                } else if (parm == "-p") {
                    strcpy(data.password.str, phrases[++i].c_str());
                } else if (parm == "-n") {
                    strcpy(data.name.str, phrases[++i].c_str());
                } else if (parm == "-m") {
                    strcpy(data.mailAddr.str, phrases[++i].c_str());
                } else if (parm == "-g") {
                    data.privilege = std::atoi(phrases[++i].c_str());
                }
            }
            if (user_system.empty()) {
                data.privilege = 10;
                cout << user_system.add_first_user(username, data);
            } else cout << user_system.add_user(cur_username, username, data);
            cout << "\n";
        } else if (order == "login") {
            String<25> username;
            String<35> pass;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-u") {
                    strcpy(username.str, phrases[++i].c_str());
                } else if (parm == "-p") {
                    strcpy(pass.str, phrases[++i].c_str());
                }
            }
            cout << user_system.login(username, pass) << "\n";
        } else if (order == "logout") {
            String<25> username;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-u") {
                    strcpy(username.str, phrases[++i].c_str());
                }
            }
            cout << user_system.logout(username) << "\n";
        } else if (order == "query_profile") {
            String<25> cur_username;
            String<25> username;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-u") {
                    strcpy(username.str, phrases[++i].c_str());
                } else if (parm == "-c") {
                    strcpy(cur_username.str, phrases[++i].c_str());
                }
            }
            cout << user_system.query_profile(cur_username, username) << "\n";
        } else if (order == "modify_profile") {
            String<25> cur_username;
            String<25> username;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-u") {
                    strcpy(username.str, phrases[++i].c_str());
                } else if (parm == "-c") {
                    strcpy(cur_username.str, phrases[++i].c_str());
                }
            }
            cout << user_system.modify_profile(cur_username, username, phrases) << "\n";
        } else if (order == "add_train") {
            String<25> id;
            train_data data;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-i") {
                    strcpy(id.str, phrases[++i].c_str());
                } else if (parm == "-n") {
                    data.station_num = std::atoi(phrases[++i].c_str());
                } else if (parm == "-m") {
                    data.seat_num = std::atoi(phrases[++i].c_str());
                } else if (parm == "-s") {
                    std::string tmp = phrases[++i];
                    int k = 0, j = 0;
                    std::string s = "";
                    while (k < tmp.size()) {
                        if (tmp[k] == '|') {
                            strcpy(data.stations[++j].str, s.c_str());
                            s = "";
                        }
                        s += tmp[k];
                        ++k;
                    }
                } else if (parm == "-p") {
                    std::string tmp = phrases[++i];
                    int k = 0, j = 0;
                    std::string s = "";
                    while (k < tmp.size()) {
                        if (tmp[k] == '|') {
                            data.prices[++j] = std::atoi(s.c_str());
                            s = "";
                        }
                        s += tmp[k];
                        ++k;
                    }
                } else if (parm == "-x") {
                    int starttime = std::atoi(phrases[++i].c_str());
                    data.leave_times[1] += starttime;
                    data.arrive_times[2] += starttime;
                    data.leave_times[2] += starttime;
                } else if (parm == "-t") {
                    std::string tmp = phrases[++i];
                    int k = 0, j = 1;
                    std::string s = "";
                    while (k < tmp.size()) {
                        if (tmp[k] == '|') {
                            data.arrive_times[++j] += std::atoi(s.c_str());
                            data.leave_times[j] += std::atoi(s.c_str());
                            s = "";
                        }
                        s += tmp[k];
                        ++k;
                    }
                } else if (parm == "-o") {
                    std::string tmp = phrases[++i];
                    int k = 0, j = 1;
                    std::string s = "";
                    while (k < tmp.size()) {
                        if (tmp[k] == '|') {
                            data.leave_times[++j] += std::atoi(s.c_str());
                            data.arrive_times[j + 1] += std::atoi(s.c_str());
                            s = "";
                        }
                        s += tmp[k];
                        ++k;
                    }
                } else if (parm == "-d") {
                    std::string tmp = phrases[++i];
                    int k = 0;
                    string star = "";
                    string end = "";
                    while (tmp[k] != '|') {
                        star += tmp[k];
                        ++k;
                    }
                    while (k < tmp.size()) {
                        end += tmp[k];
                        ++k;
                    }
                    data.start_date = Date(star);
                    data.end_date = Date(end);
                } else if (parm == "-y") {
                    data.type = phrases[++i][0];
                }
            }
            //the update of arrive and leave times
            for (int j = 2; j <= data.station_num - 1; ++j) {
                data.arrive_times[j + 1] += data.arrive_times[j];
                data.leave_times[j] += data.leave_times[j - 1];
            }
            cout << train_system.add_train(id, data) << "\n";
        } else if (order == "delete_train") {
            String<25> id;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-i") {
                    strcpy(id.str, phrases[++i].c_str());
                }
            }
            cout << train_system.delete_train(id) << "\n";
        } else if (order == "query_train") {
            String<25> id;
            std::string date;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-i") {
                    strcpy(id.str, phrases[++i].c_str());
                } else if (parm == "-d") {
                    date = phrases[++i];
                }
            }
            train_system.query_train(id, Date(date));
        } else if (order == "query_ticket") {
            Date date;
            String<40> s, t;
            std::string index;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-s") {
                    strcpy(s.str, phrases[++i].c_str());
                } else if (parm == "-t") {
                    strcpy(t.str, phrases[++i].c_str());
                } else if (parm == "-d") {
                    date = Date(phrases[++i]);
                } else if (parm == "-p") {
                    index = phrases[++i];
                }
            }
            train_system.query_ticket(date, s, t, index);
        } else if (order == "query_transfer") {
            Date date;
            String<40> s, t;
            std::string index;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-s") {
                    strcpy(s.str, phrases[++i].c_str());
                } else if (parm == "-t") {
                    strcpy(t.str, phrases[++i].c_str());
                } else if (parm == "-d") {
                    date = Date(phrases[++i]);
                } else if (parm == "-p") {
                    index = phrases[++i];
                }
            }
            train_system.query_transfer(date, s, t, index);
        } else if (order == "buy_ticket") {
            String<25> u;
            String<25> id;
            Date date;
            String<40> f, t;
            int num;
            bool q;
            while (i < siz) {
                string parm = phrases[++i];
                if (parm == "-f") {
                    strcpy(f.str, phrases[++i].c_str());
                } else if (parm == "-t") {
                    strcpy(t.str, phrases[++i].c_str());
                } else if (parm == "-d") {
                    date = Date(phrases[++i]);
                } else if (parm == "-q") {
                    if(phrases[++i]=="true") q= true;
                    else if(phrases[++i]=="false") q= false;
                }
                else if(parm=="-u"){
                    strcpy(u.str, phrases[++i].c_str());
                }
                else if(parm=="-i"){
                    strcpy(id.str, phrases[++i].c_str());
                }
                else if(parm=="-n"){
                    num=std::atoi(phrases[++i].c_str());
                }
            }
            if(!user_system.check_login(u)) {
                cout<<"-1\n";
                continue;
            }
            cout<<train_system.buy_ticket(u,id,date,f,t,num,q)<<"\n";

        }
        else if(order=="query_order"){
            String<25> u;
            string parm=phrases[++i];
            if(parm=="-u"){
                strcpy(u.str, phrases[++i].c_str());
            }
            if(!user_system.check_login(u)){
                cout<<"-1\n";
                continue;
            }
           train_system.query_order(u);
        }
        else if(order=="refund_ticket"){
            String<25> u;
            int num;
            while (i < siz) {
                string parm = phrases[++i];
                if(parm=="-u"){
                    strcpy(u.str, phrases[++i].c_str());
                }
                else if(parm=="-n"){
                    num= std::atoi(phrases[++i].c_str());
                }
            }
            if(!user_system.check_login(u)){
                cout<<"-1\n";
                continue;
            }
            train_system.refund_ticket(u, num);
        }
        else if(order=="clean"){
            user_system.clean();
            train_system.clean();
            cout<<"0\n";
        }
        else if(order=="exit"){
            exit(0);
            cout<<"bye\n";
        }
    }


}