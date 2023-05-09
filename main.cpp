#include "bpt.h"
#include <iostream>
#include "vector.h"
#include <cstring>
using namespace std;

int next_int() {
    int x;
    scanf("%d", &x);
    return x;
}
String<65> next_String() {
    String<65> x{};
    scanf("%s", x.str);
    return x;
}
int main() {
//    freopen("my_out","w",stdout);
    int n;
    bpt<String<65>, int> BPT("node_file", "leaf_file");
    n = next_int();
    while (n--) {
        auto order = std::string(next_String().str);
        if (order[0] == 'i') {

            auto index = next_String();
            auto value = next_int();
            BPT.insert(index, value);
        }
        else if (order == "delete") {
            auto index = next_String();
            auto value = next_int();
            BPT.remove(index, value);
        }
        else if (order[0] == 'f') {
            auto index = next_String();
            vector<int> ans(BPT.find(index));
            if (ans.empty()) cout << "null"
                                  << "\n";
            else {
                for (auto i = ans.begin(); i != ans.end(); ++i) {
                    cout << *i << " ";
                }
                cout << "\n";
            }
        }
    }
    return 0;
}