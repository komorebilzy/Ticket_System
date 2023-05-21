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
    String<40> stations[105];  //1_based
    int seat_num;
    int station_num;
    int prices[105];   //1_based prices[i]=stations[i]-stations[i+1]
    //this two time is calculated by startTime,travelTime and stopoverTimes
    Time leave_times[105];  //没有终点站 1->num-1
    Time arrive_times[105];   //没有起始站 2->num
    Date start_date;
    Date end_date;
    char type;
    bool released = false;

    train_data() {}

    train_data(const train_data &a) : station_num(a.station_num), seat_num(a.seat_num), start_date(a.start_date),
                                      end_date(a.end_date), released(a.released) {
        for (int i = 1; i <= station_num - 1; ++i) {
            stations[i] = a.stations[i];
            prices[i] = a.prices[i];
            leave_times[i] = a.leave_times[i];
            if (i != 1) arrive_times[i] = a.arrive_times[i];
        }
        arrive_times[station_num] = a.arrive_times[station_num];
    }

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
        String<25> train_id;
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
        String<40> transfer;
    };

    struct order_data {
        Date start_date;
        String<25> username;
        int status;    //1-success 2-pending 3-refunded
        String<25> train_id;
        String<40> from, to;
        Date leave_date;
        Time leave_time;
        Date arrive_date;
        Time arrive_time;
        int price;
        int num;
    };

    struct path_a_day {
        int a[100];
    };
    struct date_ticket {
        path_a_day b[100];
    };

    bpt<String<25>, int> train;
    std::fstream train_detail;
    int index = 0;

    bpt<String<40>, String<25>> station_name_based_train;   //station_name->train_id

    std::fstream ticket;  //id-date-station->ticket_num


    bpt<String<25>, int> order; // user to int      the int is -index1
    std::fstream order_detail;  //int->order_detail
    int index1 = 0;

    bpt<String<25>, int> pending;  //id->int

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


    void read_ticket(int id, int date, int station, int a) {
        int index_ = (id - 1) * 10000 + date * 100 + station;
        ticket.seekg(index_ * sizeof(int));
        ticket.read(reinterpret_cast<char *>(&a), sizeof(int));
    }

    void read_tickets(int id, int date, path_a_day a) {
        int index_ = (id - 1) * 10000 + date * 100;
        ticket.seekg(index_ * sizeof(int));
        ticket.read(reinterpret_cast<char *>(&a), sizeof(path_a_day));
    }

    void write_tickets(int index_, date_ticket a) {
        ticket.seekp((index_ - 1) * sizeof(a));
        ticket.write(reinterpret_cast<const char *>(&a), sizeof(a));
    }

    void write_ticket(int id, int date, path_a_day a) {
        int index_ = (id - 1) * 10000 + date * 100;
        ticket.seekp(index_ * sizeof(int));
        ticket.write(reinterpret_cast<const char *>(&a), sizeof(path_a_day));
    }


public:
    Train_system() : train("train_node", "train_leaf"),
                     station_name_based_train("s_n_b_t_node", "s_n_b_t_leaf"),
                     order("order_node", "order_leaf"),
                     pending("pending_node", "pending_leaf") {

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


    int add_train(const String<25> &id, const train_data &data) {
        auto flag = train.find(id);
        if (flag.empty()) {
            train.insert(id, ++index);
            write_train(index, data);
            return 0;
        }
        return -1;
    }

    int delete_train(const String<25> &id) {
        auto flag = train.find(id);
        if (!flag.empty()) {
            train_data data;
            read_train(flag.back(), data);
            if (data.released) {
                train.remove(id, flag.back());
                return 0;
            }
        }
        return -1;
    }

    int release_train(const String<25> &id) {
        auto flag = train.find(id);
        if (!flag.empty()) {
            train_data data;
            read_train(flag.back(), data);
            if (!data.released) {
                data.released = true;
                write_train(flag.back(), data);
                for (int i = 1; i <= data.station_num; ++i) {
                    station_name_based_train.insert(data.stations[i], id);
                }
                date_ticket date_station;
                for (int i = 0; i < data.end_date - data.start_date + 1; ++i) {
                    for (int j = 0; j < data.station_num; ++j) {
                        date_station.b[i].a[j] = data.seat_num;
                    }
                }
                write_tickets(flag.back(), date_station);
            }
            return 0;
        }
        return -1;
    }

    void query_train(const String<25> &id, const Date &d) {
        auto flag = train.find(id);
        if (!flag.empty()) {
            train_data data;
            read_train(flag.back(), data);
            if (data.start_date < d && d < data.end_date) {
                std::cout << id << " " << data.type << "\n";
                int ticket_num;
                Date date = d;
                Time time = data.leave_times[1];
                adjust(date, time);
                read_ticket(flag.back(), d - data.start_date, 0, ticket_num);
                std::cout << data.stations[1] << " xx-xx xx:xx -> " << date << " " << time
                          << " 0 " << data.seat_num << "\n";
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
                    read_ticket(flag.back(), d - data.start_date, i - 1, ticket_num);
                    std::cout << date << " " << time << " " << price << " " << ticket_num << "\n";
                }
                date = d;
                time = data.leave_times[data.station_num];
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

    bool date_change(ticket_data &ans, train_data data, int s_num, int t_num, Date curdate, Date &start) {
        Date tmp1 = data.start_date;
        Time tmp2 = data.leave_times[s_num];
        adjust(tmp1, tmp2);
        start = data.start_date + (curdate - tmp1);
        if (curdate - tmp1 < 0) return false;
        ans.leave_date = curdate;
        ans.leave_time = tmp2;
        tmp1 = start;
        tmp2 = data.arrive_times[t_num];
        adjust(tmp1, tmp2);
        ans.arrive_date = tmp1;
        ans.arrive_time = tmp2;
        ans.time = query_time(ans.leave_date, ans.leave_time, ans.arrive_date, ans.arrive_time);
        return true;
    }

    bool order_date(order_data &ans, train_data data, int s_num, int t_num, Date curdate, Date &start) {
        Date tmp1 = data.start_date;
        Time tmp2 = data.leave_times[s_num];
        adjust(tmp1, tmp2);
        start = data.start_date + (curdate - tmp1);
        if (curdate - tmp1 < 0) return false;
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
    transfer_date(ticket_data &ans, train_data data, int s_num, int t_num, Date curdate, Time curtime, Date &start) {
        Date tmp1 = data.start_date;
        Time tmp2 = data.leave_times[s_num];
        adjust(tmp1, tmp2);
        start = data.start_date + (curdate - tmp1);
        if (curdate - tmp1 < 0) return false;
        if (tmp2 < curtime) {
            start = start + 1;
            curdate = curdate + 1;
        }
        if (start > data.end_date) return false;
        ans.leave_date = curdate;
        ans.leave_time = tmp2;
        tmp1 = start;
        tmp2 = data.arrive_times[t_num];
        adjust(tmp1, tmp2);
        ans.arrive_date = tmp1;
        ans.arrive_time = tmp2;
        return true;

    }

    void cost_seat(ticket_data &ans, train_data data, String<25> id, int s_num, int t_num, Date start) {
        int price = 0;
        int seat = 100000000;
        for (int j = s_num; j < t_num; ++j) {
            price += data.prices[j];
            int seat_;
            read_ticket(train.find(id).back(), start - data.start_date, j - 1, seat_);
            seat = std::min(seat, seat_);
        }
        ans.price = price;
        ans.seat = seat;
    }

    bool compare1(transfer_data a, transfer_data b) {
        if (a.time_sum == b.time_sum) {
            return a.price_sum < b.price_sum;
        }
        return a.time_sum < b.time_sum;
    }

    bool compare2(transfer_data a, transfer_data b) {
        if (a.price_sum == b.price_sum) {
            return a.time_sum < b.time_sum;
        }
        return a.price_sum < b.price_sum;
    }

public:
    void query_ticket(const Date &date, const String<40> &s, const String<40> &t, std::string sign) {
        vector<ticket_data> tickets;
        auto ids = station_name_based_train.find(s);
        for (int i = 0; i < ids.size(); ++i) {
            //enum the id
            ticket_data ans;
            String<25> id = ids[i];
            ans.train_id = id;
            train_data data;
            read_train(train.find(id).back(), data);
            //from s to t
            bool flag = false;
            int s_num, t_num;
            for (int j = 1; j <= data.station_num; ++j) {
                if (data.stations[j] == s) {
                    s_num = j;
                    while (j <= data.station_num) {
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
//                Date tmp1 = data.start_date;
//                Time tmp2 = data.leave_times[s_num];
//                adjust(tmp1, tmp2);
//                Date start = data.start_date + (date - tmp1);
//                if (date - tmp1 < 0) continue;
//                ans.leave_date = date;
//                ans.leave_time = tmp2;
//                tmp1 = start;
//                tmp2 = data.arrive_times[t_num];
//                adjust(tmp1, tmp2);
//                ans.arrive_date = tmp1;
//                ans.arrive_time = tmp2;
//                ans.time = query_time(ans.leave_date, ans.leave_time, ans.arrive_date, ans.arrive_time);

                //price and seat
//                int price = 0;
//                int seat = 100000000;
//                for (int j = s_num; j < t_num; ++j) {
//                    price += data.prices[j];
//                    int seat_;
//                    read_ticket(train.find(id).back(), start - data.start_date, j - 1, seat_);
//                    seat = std::min(seat, seat_);
//                }
//                ans.price = price;
//                ans.seat = seat;
                cost_seat(ans, data, id, s_num, t_num, start);
                tickets.push_back(ans);
            }

        }
        int siz = tickets.size();
        int *array = new int[siz];
        for (int i = 0; i < siz; i++)
            array[i] = i;

        if (sign == "time") {
            std::sort(array, array + siz, [&tickets](int x, int y) { return tickets[x].time < tickets[y].time; });
        } else if (sign == "cost") {
            std::sort(array, array + siz, [&tickets](int x, int y) { return tickets[x].price < tickets[y].price; });
        }
        std::cout << siz << "\n";
        for (int i = 0; i < siz; ++i) {
            ticket_data ans = tickets[array[i]];
            std::cout << ans.train_id << " " << s << " " << ans.leave_date << " " << ans.leave_time << " -> " << t
                      << " " << ans.arrive_date << " " << ans.arrive_time << " " << ans.price << " " << ans.seat
                      << "\n";
        }
    }

    void query_transfer(const Date &date, const String<40> &s, const String<40> &t, std::string sign) {
        vector<transfer_data> ans;
        auto ids1 = station_name_based_train.find(s);
        auto ids3 = station_name_based_train.find(t);
        for (int i = 0; i < ids1.size(); ++i) {
            ticket_data ans1;
            String<40> transfer;
            String<25> id1 = ids1[i];
            train_data data1;
            read_train(train.find(id1).back(), data1);
            //the first train
            int s1_num, t1_num;
            Date start1;
            for (int j = 1; j <= data1.station_num; ++j) {
                if (data1.stations[j] == s) {
                    //transfer
                    s1_num = j;
                    while (++j <= data1.station_num) {
                        t1_num = j;
                        transfer = data1.stations[j];
                        auto ids2 = station_name_based_train.find(transfer);
                        for (int k = 0; k < ids2.size(); ++k) {
                            ticket_data ans2;
                            train_data data2;
                            String<25> id2 = ids2[k];
                            bool if_reach = false;
                            for (int x = 0; x < ids3.size(); ++x) {
                                if (ids3[x] == id2) {
                                    if_reach = true;
                                    break;
                                }
                            }
                            if (id2 == id1 || !if_reach) continue;
                            read_train(train.find(id2).back(), data2);
                            //the second train
                            int s2_num, t2_num;
                            Date start2;
                            for (int m = 1; m <= data2.station_num; ++m) {
                                if (data2.stations[m] == transfer) {
                                    s2_num = m;
                                    while (m <= data2.station_num) {
                                        if (data2.stations[++m] == t) {
                                            t2_num = m;
                                        }
                                    }
                                }
                            }
                            //date and time
                            if (!date_change(ans1, data1, s1_num, t1_num, date, start1)) continue;
                            if (!transfer_date(ans2, data2, s2_num, t2_num, ans1.arrive_date, ans1.arrive_time,
                                               start2))
                                continue;
                            //price and seat
                            cost_seat(ans1, data1, id1, s1_num, t1_num, start1);
                            cost_seat(ans2, data2, id2, s2_num, t2_num, start2);
                            transfer_data tmp;
                            tmp.transfer = transfer;
                            tmp.first = ans1;
                            tmp.second = ans2;
                            tmp.price_sum = ans1.price + ans2.price;
                            tmp.time_sum = query_time(ans1.leave_date, ans1.leave_time, ans2.arrive_date,
                                                      ans2.arrive_time);
                            ans.push_back(tmp);

                        }
                    }
                }
            }
        }
        if (ans.empty()) {
            std::cout << "0" << "\n";
            return;
        } else {
            transfer_data final = ans[0];
            if (sign == "time") {
                for (int i = 1; i < ans.size(); ++i) {
                    if (compare1(final, ans[i])) final = ans[i];
                }
            } else if (sign == "cost") {
                for (int i = 1; i < ans.size(); ++i) {
                    if (compare2(final, ans[i])) final = ans[i];
                }
            }
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


    std::string buy_ticket(String<25> user, String<25> id, Date d, String<40> f, String<40> t, int num, bool q) {
        auto flag = train.find(id);
        if (flag.empty()) return "-1";
        train_data data;
        order_data orders;
        Date start;
        read_train(flag.back(), data);
        if (!data.released) return "-1";

        int f_num, t_num, price = 0;
        for (int j = 1; j <= data.station_num; ++j) {
            if (data.stations[j] == f) {
                f_num = j;
                while (j <= data.station_num) {
                    price += data.prices[j];
                    if (data.stations[++j] == t) {
                        t_num = j;
                        j = data.station_num + 1;
                    }
                }
            }
        }
        orders.price = price;
        orders.username = user;
        orders.train_id = id;
        orders.num = num;
        orders.from = f;
        orders.to = t;
        if (!order_date(orders, data, f_num, t_num, d, start)) return "-1";
        orders.start_date = start;
        int seat_num = 1000000000;
        //可以直接read或者write一个数组 求数组最小值
        path_a_day tmp;
        read_tickets(flag.back(), start - data.start_date, tmp);
        for (int i = f_num; i < t_num; ++i) {
            if (tmp.a[i] < seat_num) seat_num = tmp.a[i];
        }
        if (seat_num >= num) {
            for (int i = f_num; i < t_num; ++i) {
                tmp.a[i] -= num;
            }
            write_ticket(flag.back(), start - data.start_date, tmp);
            orders.status = 1;
            write_orders(++index1, orders);
            order.insert(user, index1 * -1);
            return std::to_string(price);
        }
        if (!q) return "-1";
        else {
            orders.status = 2;
            write_orders(++index1, orders);
            order.insert(user, index1 * -1);
            pending.insert(id, index1 * -1);
            return "pending";
        }
    }

    void query_order(String<25> user) {
        auto list = order.find(user);
        std::cout << list.size() << "\n";
        order_data data;
        for (int i = 0; i < list.size(); ++i) {
            read_orders((-1) * list[i], data);
            std::cout << data.status << " " << data.train_id << " " << data.from << ' ' << data.leave_date << " "
                      << data.leave_time << " -> " << data.to << " " << data.arrive_date << " " << data.arrive_time
                      << " " << data.price << " " << data.num << "\n";
        }
    }

private:
    bool refund_buy(order_data p) {
        auto flag = train.find(p.train_id);
        train_data data;
        read_train(flag.back(), data);
        path_a_day tmp;
        read_tickets(flag.back(), p.leave_date - p.start_date, tmp);
        int f_num, t_num, price = 0;
        for (int j = 1; j <= data.station_num; ++j) {
            if (data.stations[j] == p.from) {
                f_num = j;
                while (j <= data.station_num) {
                    if (data.stations[++j] == p.to) {
                        t_num = j;
                        j = data.station_num + 1;
                    }
                }
            }
        }
        int seat_num = 1000000000;
        for (int i = f_num; i < t_num; ++i) {
            if (tmp.a[i] < seat_num) seat_num = tmp.a[i];
        }
        if (seat_num >= p.num) {
            for (int i = f_num; i < t_num; ++i) {
                tmp.a[i] -= p.num;
            }
            write_ticket(flag.back(), p.leave_date - p.start_date, tmp);
            p.status = 3;
            return true;
        } else return false;
    }

public:
    void refund_ticket(String<25> u, int num) {
        auto list = order.find(u);
        order_data data;   //退订订单
        read_orders((-1) * list[num - 1], data);
        if (data.status == 3) std::cout << "-1\n";
        else if (data.status == 2) {
            data.status = 3;
            write_orders((-1) * list[num - 1], data);
            pending.remove(data.train_id, (-1) * list[num - 1]);
            std::cout << "0\n";
        } else if (data.status == 1) {
            data.status = 3;
            std::cout << "0\n";
            write_orders((-1) * list[num - 1], data);
            path_a_day tmp;
            read_tickets(train.find(data.train_id).back(), data.leave_date - data.start_date, tmp);
            for (int i = 0; i < 100; ++i) {
                tmp.a[i] += data.num;
            }
            write_ticket(train.find(data.train_id).back(), data.leave_date - data.start_date, tmp);
            auto ans = pending.find(data.train_id);   //可能符合条件的候补订单
            order_data pos;
            for (int i = 0; i < ans.size(); ++i) {
                read_orders(ans[i] * (-1), pos);
                if (refund_buy(pos)) {
                    write_orders((-1) * ans[i], pos);
                    pending.remove(pos.train_id, ans[i] * (-1));
                    return;
                }
//                buy_ticket(u, data.train_id, data.leave_date, data.from, data.to, data.num, true);
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