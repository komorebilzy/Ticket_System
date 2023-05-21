#ifndef TICKET_SYSTEM_MYSTRING_H
#define TICKET_SYSTEM_MYSTRING_H



template<int T>
struct String {
    char str[T];

    bool operator<(const String &a) const {
        return strcmp(str, a.str) < 0;
    }

    bool operator==(const String &a) const {
        return strcmp(str, a.str) == 0;
    }

    bool operator!=(const String &a) const {
        return !((*this) == a);
    }

    bool operator<=(const String &a) const {
        return (*this < a || *this == a);
    }

    bool operator>(const String &a) const {
        return strcmp(str, a.str) > 0;
    }

    bool operator>=(const String &a) const {
        return (*this > a || *this == a);
    }

    friend std::istream &operator>>(std::istream &is, String &tmp) {
        return is >> tmp.str;
    }

    friend std::ostream &operator<<(std::ostream &os, const String &tmp) {
        return os << tmp.str;
    }

    String<T> &operator=(const String<T> &rhs) {
        if (this != &rhs) { memcpy(str, rhs.str, sizeof(str)); }
        return *this;
    }

    String()=default;
};

#endif //TICKET_SYSTEM_MYSTRING_H
