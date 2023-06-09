#ifndef TICKET_SYSTEM_TRAIN_SYSTEM_H
#define TICKET_SYSTEM_TRAIN_SYSTEM_H

#include <cstring>
#include <iostream>
#include "MyString.h"
#include "bpt.h"
#include "map.h"
#include "Data.h"
#include <algorithm>

struct train_data {
    bool released;
    char type;
    int seat_num;
    int prices[101];   //1_based prices[i]=stations[i]-stations[i+1]
    //this two time is calculated by startTime,travelTime and stopoverTimes
    String<21> id;
    Date start_date;
    Date end_date;
    Time leave_times[101];  //没有终点站 1->num-1
    Time arrive_times[101];   //没有起始站 2->num
    int station_num;
    String<31> stations[101];  //1_based


    friend bool operator==(const train_data &a, const train_data &b) {
        return a.seat_num == b.seat_num && a.station_num == b.station_num && a.type == b.type;
    }

    friend bool operator<=(const train_data &a, const train_data &b) {
        return true;
    }

    friend bool operator<(const train_data &a, const train_data &b) {
        return false;
    }

    friend bool operator>(const train_data &a, const train_data &b) {
        return false;
    }

    friend bool operator>=(const train_data &a, const train_data &b) {
        return true;
    }

};

class Train_system {
private:


    struct ticket_data {
        int time;
        int price;
        int seat;
        String<21> train_id;
        Date leave_date;
        Time leave_time;
        Date arrive_date;
        Time arrive_time;
    };

    struct transfer_data {
        int time_sum;
        int price_sum;
        ticket_data first;
        ticket_data second;
        String<31> transfer;
    };

    struct order_data {
        int f_num, t_num;
        Date start;
        Date start_date;
        String<21> username;
        int status;    //1-success 2-pending 3-refunded
        String<21> train_id;
        String<31> from, to;
        Date leave_date;
        Time leave_time;
        Date arrive_date;
        Time arrive_time;
        int price;
        int num;
    };

    struct query_tra {
        int id1;
        int id2;
        String<31> transfer;

        query_tra(int id1_, int id2_, const String<31> trans) : id1(id1_), id2(id2_), transfer(trans) {}
    };

    struct path_a_day {
        int a[100];
    };
    struct date_ticket {
        path_a_day b[100];
    };

    bpt<String<21>, int> train{"train_node", "train_leaf"};
    std::fstream train_detail;
    int index = 0;

    bpt<String<31>, int> station_name_based_train{"s_n_b_t_node", "s_n_b_t_leaf"};   //station_name->train_id

    std::fstream ticket;  //id-date-station->ticket_num  0-based


    bpt<String<21>, int> order{"order_node", "order_leaf"}; // user to int      the int is -index1
    std::fstream order_detail;  //int->order_detail
    int index1 = 0;

    bpt<String<21>, int> pending{"pending_node", "pending_leaf"};  //id->int

    void read_train(int index_, train_data &m) {
        train_detail.seekg(sizeof(int) + (index_ - 1) * sizeof(train_data));
        train_detail.read(reinterpret_cast<char *>(&m), sizeof(train_data));
    }

    void write_train(int index_, const train_data &m) {
        train_detail.seekp((index_ - 1) * sizeof(train_data) + sizeof(int));
        train_detail.write(reinterpret_cast<const char *>(&m), sizeof(train_data));
    }

    void read_orders(int index_, order_data &m) {
        order_detail.seekg(sizeof(int) + (index_ - 1) * sizeof(order_data));
        order_detail.read(reinterpret_cast<char *>(&m), sizeof(order_data));
    }

    void write_orders(int index_, order_data &m) {
        order_detail.seekp(sizeof(int) + (index_ - 1) * sizeof(order_data));
        order_detail.write(reinterpret_cast<const char *>(&m), sizeof(order_data));
    }


    void read_ticket(int id, int date, int station, int &a) {
        int index_ = (id - 1) * 10000 + date * 100 + station;
        ticket.seekg(index_ * sizeof(int));
        ticket.read(reinterpret_cast<char *>(&a), sizeof(int));
    }

    void read_tickets(int id, int date, path_a_day &a) {
        int index_ = (id - 1) * 10000 + date * 100;
        ticket.seekg(index_ * sizeof(int));
        ticket.read(reinterpret_cast<char *>(&a), sizeof(path_a_day));
    }

    void write_tickets(int id, date_ticket a) {
        ticket.seekp((id - 1) * sizeof(a));
        ticket.write(reinterpret_cast<const char *>(&a), sizeof(a));
    }

    void write_ticket(int id, int date, path_a_day a) {
        int index_ = (id - 1) * 10000 + date * 100;
        ticket.seekp(index_ * sizeof(int));
        ticket.write(reinterpret_cast<const char *>(&a), sizeof(path_a_day));
    }


public:
    Train_system() {

        std::fstream in1("train_detail");
        if (!in1) {
            std::ofstream out("train_detail");
            out.close();
            train_detail.open("train_detail");
            train_detail.seekp(0);
            train_detail.write(reinterpret_cast<const char *>(&index), sizeof(int));
            train_detail.close();
        }
        train_detail.open("train_detail");
        train_detail.seekg(0);
        train_detail.read(reinterpret_cast<char *>(&index), sizeof(int));


        std::ifstream in2("ticket");
        if (!in2) {
            std::ofstream out("ticket");
            out.close();
        }
        in2.close();
        ticket.open("ticket");


        std::ifstream in3("order_detail");
        if (!in3) {
            std::ofstream out("order_detail");
            out.close();
            order_detail.open("order_detail");
            order_detail.seekp(0);
            order_detail.write(reinterpret_cast<const char *>(&index1), sizeof(int));
            order_detail.close();
        }
        in3.close();
        order_detail.open("order_detail");
        order_detail.seekp(0);
        order_detail.read(reinterpret_cast<char *>(&index1), sizeof(int));


    }

    ~Train_system() {
        train_detail.seekp(0);
        train_detail.write(reinterpret_cast<char *>(&index), sizeof(int));
        train_detail.close();

        order_detail.seekp(0);
        order_detail.write(reinterpret_cast<char *>(&index1), sizeof(int));
        order_detail.close();

        ticket.close();
    }


    int add_train(const String<21> &id, const train_data &data) {
        auto flag = train.find(id);
        if (flag.empty()) {
            train.insert(id, ++index);
            write_train(index, data);
            return 0;
        }
        return -1;
    }

    int delete_train(const String<21> &id) {
        auto flag = train.find(id);
        if (!flag.empty()) {
            bool released;
            train_detail.seekg(sizeof(int) + (flag.back() - 1) * sizeof(train_data));
            train_detail.read(reinterpret_cast<char *>(&released), sizeof(bool));
            if (!released) {
                train.remove(id, flag.back());
                return 0;
            }
        }
        return -1;
    }

    int release_train(const String<21> &id) {
        auto flag = train.find(id);
        if (!flag.empty()) {
            train_data data;
            read_train(flag.back(), data);
            if (!data.released) {
                data.released = true;
                write_train(flag.back(), data);
                for (int i = 1; i <= data.station_num; ++i) {
                    station_name_based_train.insert(data.stations[i], flag.back());
                }
                date_ticket date_station;
                for (int i = 0; i < data.end_date - data.start_date + 1; ++i) {
                    for (int j = 0; j < data.station_num - 1; ++j) {
                        date_station.b[i].a[j] = data.seat_num;
                    }
                }
                write_tickets(flag.back(), date_station);
                return 0;
            }
        }
        return -1;
    }

    void query_train(const String<21> &id, const Date &d) {
        auto flag = train.find(id);
        if (!flag.empty()) {
            train_data data;
            read_train(flag.back(), data);
            if (data.start_date <= d && d <= data.end_date) {
                std::cout << id << " " << data.type << "\n";
                int ticket_num;
                Date date = d;
                Time time = data.leave_times[1];
                adjust(date, time);
                if (!data.released) ticket_num = data.seat_num;
                else read_ticket(flag.back(), d - data.start_date, 0, ticket_num);
                std::cout << data.stations[1] << " xx-xx xx:xx -> " << date << " " << time
                          << " 0 " << ticket_num << "\n";
                int price = 0;
                for (int i = 2; i <= data.station_num - 1; ++i) {
                    price += data.prices[i - 1];
                    date = d;
                    time = data.arrive_times[i];
                    adjust(date, time);
                    std::cout << data.stations[i] << " " << date << " " << time << " -> ";
                    date = d;
                    time = data.leave_times[i];
                    adjust(date, time);
                    if (!data.released) ticket_num = data.seat_num;
                    else read_ticket(flag.back(), d - data.start_date, i - 1, ticket_num);
                    std::cout << date << " " << time << " " << price << " " << ticket_num << "\n";
                }
                date = d;
                time = data.arrive_times[data.station_num];
                adjust(date, time);
                price += data.prices[data.station_num - 1];
                std::cout << data.stations[data.station_num] << " " << date << " " << time << " -> xx-xx xx:xx "
                          << price << " x\n";
                return;
            }
            std::cout << -1 << "\n";
        }
    }

private:

    bool date_change(ticket_data &ans, const train_data &data, int s_num, int t_num, Date curdate, Date &start) {
        Date tmp1 = data.start_date;
        Time tmp2 = data.leave_times[s_num];
        adjust(tmp1, tmp2);
        start = data.start_date + (curdate - tmp1);
        if (curdate - tmp1 < 0 || start > data.end_date) return false;
        ans.leave_date = curdate;
        ans.leave_time = tmp2;
        tmp1 = start;
        tmp2 = data.arrive_times[t_num];
        adjust(tmp1, tmp2);
        ans.arrive_date = tmp1;
        ans.arrive_time = tmp2;
        ans.time = query_time(ans.leave_date, ans.leave_time, ans.arrive_date, ans.arrive_time);
//        std::cout<<"test "<<ans.leave_date<<" "<<ans.arrive_date<<" "<<start<<"\n";
        return true;
    }

    bool order_date(order_data &ans, const train_data &data, int s_num, int t_num, Date curdate, Date &start) {
        Date tmp1 = data.start_date;
        Time tmp2 = data.leave_times[s_num];
        adjust(tmp1, tmp2);
        start = data.start_date + (curdate - tmp1);
        if (curdate - tmp1 < 0 || start > data.end_date)
            return false;
        ans.leave_date = curdate;
        ans.leave_time = tmp2;
        tmp1 = start;
        tmp2 = data.arrive_times[t_num];
        adjust(tmp1, tmp2);
        ans.arrive_date = tmp1;
        ans.arrive_time = tmp2;
        return true;
    }


    bool
    transfer_date(ticket_data &ans, const train_data &data, int s_num, int t_num, Date curdate, const Time &curtime,
                  Date &start) {
        Date tmp1 = data.start_date;
        Time tmp2 = data.leave_times[s_num];
        adjust(tmp1, tmp2);
//        if (curdate - tmp1 < 0) return false;
        Date leave = curdate;
        if (curdate < tmp1) {
            start = data.start_date;
            leave = tmp1;
        } else start = data.start_date + (curdate - tmp1);
        if (curdate == leave && tmp2 < curtime) {
            start = start + 1;
            leave = leave + 1;
        }
        if (start > data.end_date) return false;
        ans.leave_date = leave;
        ans.leave_time = tmp2;
        tmp1 = start;
        tmp2 = data.arrive_times[t_num];
        adjust(tmp1, tmp2);
        ans.arrive_date = tmp1;
        ans.arrive_time = tmp2;
        return true;

    }

    void cost_seat(ticket_data &ans, const train_data &data, int pos, int s_num, int t_num, Date start) {
        int price = 0;
        int seat = 100000000;
        for (int j = s_num; j < t_num; ++j) {
            price += data.prices[j];
            int seat_;
            read_ticket(pos, start - data.start_date, j - 1, seat_);
            seat = std::min(seat, seat_);
        }
        ans.price = price;
        ans.seat = seat;
    }

    inline bool compare1(transfer_data a, transfer_data b) {
        if (a.time_sum != b.time_sum) return a.time_sum < b.time_sum;
        if (a.price_sum != b.price_sum) return a.price_sum < b.price_sum;
        if (a.first.train_id != b.first.train_id) return a.first.train_id < b.first.train_id;
        return a.second.train_id < b.second.train_id;
    }

    inline bool compare2(transfer_data a, transfer_data b) {
        if (a.price_sum != b.price_sum) return a.price_sum < b.price_sum;
        if (a.time_sum != b.time_sum) return a.time_sum < b.time_sum;
        if (a.first.train_id != b.first.train_id) return a.first.train_id < b.first.train_id;
        return a.second.train_id < b.second.train_id;
    }

public:
    void
    query_ticket(const Date &date, const String<31> &s, const String<31> &t, std::string sign) {
        vector<ticket_data> tickets;
        auto ids = station_name_based_train.find(s);
        auto ids1 = station_name_based_train.find(t);
        map<int, bool> query;
        vector<int> right_train;

        for (int i = 0; i < ids.size(); ++i) {
            query.insert(std::pair<int, bool>(ids[i], true));
        }
        for (int i = 0; i < ids1.size(); ++i) {
            if (query.find(ids1[i]) != query.end()) right_train.push_back(ids1[i]);
        }

        train_data data;
        for (int i = 0; i < right_train.size(); ++i) {
            //enum the id
            ticket_data ans;
            read_train(right_train[i], data);

            ans.train_id = data.id;
            if (!data.released) continue;
            //from s to t
            bool flag = false;
            int s_num, t_num;
            for (int j = 1; j <= data.station_num; ++j) {
                if (data.stations[j] == s) {
                    s_num = j;
                    while (j < data.station_num) {
                        if (data.stations[++j] == t) {
                            flag = true;
                            t_num = j;
                            j = data.station_num + 1;
                        }
                    }
                }
            }
            //date of -t
            if (flag) {
                Date start;
                if (!date_change(ans, data, s_num, t_num, date, start)) continue;
                cost_seat(ans, data, right_train[i], s_num, t_num, start);
                tickets.push_back(ans);
            }
        }
        int siz = tickets.size();
        int *array = new int[siz];
        for (int i = 0; i < siz; i++)
            array[i] = i;

        if (sign == "time") {
            std::sort(array, array + siz, [&tickets](int x, int y) {
                return tickets[x].time < tickets[y].time ||
                       tickets[x].time == tickets[y].time && tickets[x].train_id < tickets[y].train_id;
            });
        } else if (sign == "cost") {
            std::sort(array, array + siz, [&tickets](int x, int y) {
                return tickets[x].price < tickets[y].price ||
                       tickets[x].price == tickets[y].price && tickets[x].train_id < tickets[y].train_id;
            });
        }
        std::cout << siz << "\n";
        for (int i = 0; i < siz; ++i) {
            ticket_data ans = tickets[array[i]];
            std::cout << ans.train_id << " " << s << " " << ans.leave_date << " " << ans.leave_time << " -> " << t
                      << " " << ans.arrive_date << " " << ans.arrive_time << " " << ans.price << " " << ans.seat
                      << "\n";
        }
    }

    void query_transfer(const Date &date, const String<31> &s, const String<31> &t, std::string sign,
                        std::string timeOrder) {
//        vector<transfer_data> ans;
        auto ids1 = station_name_based_train.find(s);
        auto ids3 = station_name_based_train.find(t);
        train_data data1;
        train_data data2;
        if(ids1.empty()||ids3.empty()) {
            std::cout << "0" << "\n";
            return;
        }
        map<String<31>, vector<int>> query_trans;
        vector<query_tra> info;
        static String<31> station[101];
        for (int i = 0; i < ids1.size(); ++i) {
            int num;
            train_detail.seekg(sizeof(int) + (ids1[i] - 1) * sizeof(train_data) + 2068);
            train_detail.read(reinterpret_cast<char *> (&num),4);
            train_detail.read(reinterpret_cast<char *>(&station), sizeof(station));
            for (int j = 1; j <= num; ++j) {
                if (station[j] == s) {
                    while (j < num) query_trans[station[++j]].push_back(ids1[i]);
                }
            }
        }

        for (int i = 0; i < ids3.size(); ++i) {
            int num=0;
            train_detail.seekg(sizeof(int) + (ids3[i] - 1) * sizeof(train_data) + 2068);
            train_detail.read(reinterpret_cast<char *> (&num),4);
            train_detail.read(reinterpret_cast<char *>(&station), sizeof(station));
            int pos=0;
            for (int j = 1; j <= num; ++j) {
                if (station[j] == t) {
                    pos = j;
                    break;
                }
            }
            for (int j = 0; j < pos; ++j) {
                auto p = query_trans.find(station[j]);
                if (p == query_trans.end()) continue;
                vector<int> const &first_train = p->second;
                for (int k = 0; k < first_train.size(); ++k) {
                    if(first_train[k]==ids3[i]) continue;
                    info.push_back(query_tra(first_train[k], ids3[i], station[j]));
                }
            }
        }
        ticket_data ans1;
        ticket_data ans2;
        transfer_data final;
        bool exist= false;
        auto compare_fun = (sign == "time" ? &Train_system::compare1 : &Train_system::compare2);
        for(int i=0;i<info.size();++i){
            read_train(info[i].id1,data1);
            read_train(info[i].id2,data2);
            if(!data1.released || !data2.released) continue;
            ans1.train_id = data1.id;
            ans2.train_id = data2.id;
            int s1_num, t1_num;
            int s2_num, t2_num;
            bool flag1= false;
            bool flag2= false;
            for(int j=1;j<data1.station_num;++j){
                if(data1.stations[j]==s){
                    s1_num=j;
                    while (j < data1.station_num) {
                        if (data1.stations[++j] == info[i].transfer) {
                            flag1=true;
                            t1_num = j;
                            j = data1.station_num + 1;
                        }
                    }
                }
            }
            for(int j=1;j<data2.station_num;++j){
                if(data2.stations[j]==info[i].transfer){
                    s2_num=j;
                    while (j < data2.station_num) {
                        if (data2.stations[++j] == t) {
                            flag2=true;
                            t2_num = j;
                            j = data2.station_num + 1;
                        }
                    }
                }
            }
            if(!flag2||!flag1) continue;
            Date start1,start2;
            if (!date_change(ans1, data1, s1_num, t1_num, date, start1)) continue;
            if (!transfer_date(ans2, data2, s2_num, t2_num, ans1.arrive_date, ans1.arrive_time,
                               start2))
                continue;
            //price and seat
            cost_seat(ans1, data1, info[i].id1, s1_num, t1_num, start1);
            cost_seat(ans2, data2, info[i].id2, s2_num, t2_num, start2);
            transfer_data tmp;
            tmp.transfer = info[i].transfer;
            tmp.first = ans1;
            tmp.second = ans2;
            tmp.price_sum = ans1.price + ans2.price;
            tmp.time_sum = query_time(ans1.leave_date, ans1.leave_time, ans2.arrive_date,
                                      ans2.arrive_time);

//            ans.push_back(tmp);
            if(!exist){
                exist=true;
                final=tmp;
            }
            else {
                if(sign=="time")  {
                    if (compare1(tmp, final)) final = tmp;
                }
                else if(sign=="cost"){
                    if (compare2(tmp, final)) final = tmp;
                }
            }
        }

//        auto compare_fun = (sign == "time" ? &Train_system::compare1 : &Train_system::compare2);
//
//        if (this->*compare_fun(ans[i], final))
        if (!exist) {
            std::cout << "0" << "\n";
            return;
        } else {
            ticket_data answer;
            answer = final.first;
            std::cout << answer.train_id << " " << s << " " << answer.leave_date << " " << answer.leave_time << " -> "
                      << final.transfer << " " << answer.arrive_date << " " << answer.arrive_time << " " << answer.price
                      << " " << answer.seat << "\n";
            answer = final.second;
            std::cout << answer.train_id << " " << final.transfer << " " << answer.leave_date << " "
                      << answer.leave_time << " -> "
                      << t << " " << answer.arrive_date << " " << answer.arrive_time << " " << answer.price
                      << " " << answer.seat << "\n";
        }
    }


    std::string buy_ticket(String<21> user, String<21> id, Date d, String<31> f, String<31> t, int num, bool q,
                           std::string timeOrder) {
        auto flag = train.find(id);

        if (flag.empty()) return "-1";
        train_data data;
        order_data orders;
        Date start;
        read_train(flag.back(), data);
        if (!data.released || num > data.seat_num) return "-1";

        int f_num, t_num, price = 0;
        bool exist = false;

        for (int j = 1; j <= data.station_num; ++j) {
            if (data.stations[j] == f) {
                f_num = j;
                while (j < data.station_num) {
                    price += data.prices[j];
                    if (data.stations[++j] == t) {
                        t_num = j;
                        exist = true;
                        j = data.station_num + 1;
                    }
                }
            }
        }
        if (!exist) return "-1";
        if (!order_date(orders, data, f_num, t_num, d, start)) return "-1";
        orders.price = price;
        orders.username = user;
        orders.train_id = id;
        orders.num = num;
        orders.from = f;
        orders.to = t;
        orders.f_num = f_num;
        orders.t_num = t_num;
        orders.start_date = data.start_date;
        orders.start = start;
        int seat_num = 1000000000;
        //可以直接read或者write一个数组 求数组最小值
        path_a_day tmp;

        read_tickets(flag.back(), start - data.start_date, tmp);

        for (int i = f_num; i < t_num; ++i) {
            if (tmp.a[i - 1] < seat_num) seat_num = tmp.a[i - 1];
        }

        if (seat_num >= num) {
            for (int i = f_num; i < t_num; ++i) {
                tmp.a[i - 1] -= num;
            }
            write_ticket(flag.back(), start - data.start_date, tmp);
            orders.status = 1;
            write_orders(++index1, orders);
            order.insert(user, index1 * -1);
            return std::to_string(price * num);
        }
        if (!q) return "-1";
        else {
            orders.status = 2;
            write_orders(++index1, orders);
            order.insert(user, index1 * -1);
            pending.insert(id, index1);
            return "queue";
        }
    }

    void query_order(String<21> user) {
        auto list = order.find(user);
        std::cout << list.size() << "\n";
        order_data data;
        for (int i = 0; i < list.size(); ++i) {
            read_orders((-1) * list[i], data);
            if (data.status == 1) std::cout << "[success] ";
            else if (data.status == 2) std::cout << "[pending] ";
            else if (data.status == 3) std::cout << "[refunded] ";
            std::cout << data.train_id << " " << data.from << ' ' << data.leave_date << " "
                      << data.leave_time << " -> " << data.to << " " << data.arrive_date << " " << data.arrive_time
                      << " " << data.price << " " << data.num << "\n";
        }
    }

private:
    bool refund_buy(order_data &p) {
        auto flag = train.find(p.train_id);
        path_a_day tmp;
        read_tickets(flag.back(), p.start - p.start_date, tmp);
        int seat_num = 1000000000;
        for (int i = p.f_num; i < p.t_num; ++i) {
            if (tmp.a[i - 1] < seat_num) seat_num = tmp.a[i - 1];
        }
        if (seat_num >= p.num) {
            for (int i = p.f_num; i < p.t_num; ++i) {
                tmp.a[i - 1] -= p.num;
            }
            write_ticket(flag.back(), p.start - p.start_date, tmp);
            p.status = 1;
            return true;
        }
        return false;
    }

public:
    void refund_ticket(String<21> u, int num) {
        auto list = order.find(u);
        if (list.empty() || num > list.size()) {
            std::cout << "-1\n";
            return;
        }
        order_data data;   //退订订单
        read_orders((-1) * list[num - 1], data);
        if (data.status == 3) {
            std::cout << "-1\n";
            return;
        } else if (data.status == 2) {
            data.status = 3;
            write_orders((-1) * list[num - 1], data);
            pending.remove(data.train_id, (-1) * list[num - 1]);
            std::cout << "0\n";
        } else if (data.status == 1) {
            data.status = 3;
            std::cout << "0\n";
            write_orders((-1) * list[num - 1], data);
            path_a_day tmp;
            read_tickets(train.find(data.train_id).back(), data.start - data.start_date, tmp);
            for (int i = data.f_num; i < data.t_num; ++i) {
                tmp.a[i - 1] += data.num;
            }
            write_ticket(train.find(data.train_id).back(), data.start - data.start_date, tmp);
            auto ans = pending.find(data.train_id);   //可能符合条件的候补订单
            order_data pos;
            for (int i = 0; i < ans.size(); ++i) {
                read_orders(ans[i], pos);
                if (refund_buy(pos)) {
//                    std::cout<<pos.username<<" "<<pos.train_id <<pos.from<<" "<<pos.to<<" "<<pos.status;
                    write_orders(ans[i], pos);
                    pending.remove(pos.train_id, ans[i]);
                }
            }
        }
    }

    void clean() {
        train.clean();
        station_name_based_train.clean();
        order.clean();
        pending.clean();
        index = index1 = 0;
    }


};


#endif //TICKET_SYSTEM_TRAIN_SYSTEM_H
