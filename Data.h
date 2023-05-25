#ifndef TICKET_SYSTEM_DATA_H
#define TICKET_SYSTEM_DATA_H


class Date {

public:
    int m;
    int d;

    Date() =default;

    Date(const std::string &a) {
        m = (a[1] - '0');
        d = 10 * (a[3] - '0') + (a[4] - '0');
    }

    void operator++() {
        d++;
        if (m == 6 && d > 30) {
            d -= 30;
            m += 1;
        } else if (d > 31) {
            d -= 31;
            m += 1;
        }
    }


    friend Date operator+(Date tmp, const int x) {
        Date ans = tmp;
        ans.d += x;
        while (ans.d > 31 || ans.m == 6 && ans.d > 30)
            if (ans.m == 6 && ans.d > 30) {
                ans.m += 1;
                ans.d -= 30;
            } else if (ans.d > 31) {
                ans.m += 1;
                ans.d -= 31;
            }
        return ans;
    }

    void operator--() {
        d--;
        if (d == 0) {
            if (m == 7) {
                d = 30;
            } else {
                d = 31;
            }
            m--;
        }
    }


    friend int operator-(Date a, Date b) {
        if (a > b) {
            int res = 0;
            while (a.m > b.m) {
                res += a.d;
                a.d = 1;
                --a;
            }
            return res + a.d - b.d;
        } else if (a == b) {
            return 0;
        } else if (a < b) {
            return -1 * (b - a);
        }
    }


    friend std::ostream &operator<<(std::ostream &os, const Date &date) {
        return os << '0' << date.m << '-' << date.d / 10 << date.d % 10;
    }

    friend bool operator==(const Date &a, const Date &b) {
        return b.m == a.m && b.d == a.d;
    }

    friend bool operator<(const Date &a, const Date &b) {
        return a.m < b.m || a.m == b.m && a.d < b.d;
    }

    friend bool operator>(const Date &a, const Date &b) {
        return !(a == b || a < b);
    }

    friend bool operator<=(const Date &a, const Date &b) {
        return a == b || a < b;
    }

    friend bool operator>=(const Date &a, const Date &b) {
        return a == b || a > b;
    }

};


class Time {
public:
    int h = 0;
    int m = 0;

    Time() =default;

    Time(const std::string &a) {
        h = 10 * (a[0] - '0') + (a[1] - '0');
        m = 10 * (a[3] - '0') + (a[4] - '0');
    }


    void operator++() {
        m++;
        if (m >= 60) {
            m -= 60;
            h += 1;
        }
    }

    void operator+=(int x) {
        m += x;
        if (m >= 60) {
            h += m / 60;
            m %= 60;
        }
    }

    void operator+=(const Time &a) {
        h += a.h;
        m += a.m;
        h += m / 60;
        m %= 60;
    }

    friend int operator-(const Time &a, const Time &x) {
        return 60 * (a.h - x.h) + (a.m - x.m);
    }

    friend bool operator<(const Time &a, const Time &b) {
        return a.h < b.h || a.h == b.h && a.m < b.m;
    }


    friend std::ostream &operator<<(std::ostream &os, const Time &time) {
        return os << time.h / 10 << time.h % 10 << ':' << time.m / 10 << time.m % 10;
    }

    friend bool operator==(const Time &a, const Time &b) {
        return a.h == b.h && a.m == b.m;
    }

};


void adjust(Date &d, Time &t) {
    while (t.h >= 24) {
        ++d;
        t.h -= 24;
    }
}


int query_time(Date d1, Time t1, Date d2, Time t2) {
    if (t2 < t1) {
        t2.h += 24;
        --d2;
    }
    return (d2 - d1) * 1440 + (t2 - t1);
}

#endif //TICKET_SYSTEM_DATA_H
