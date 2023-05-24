#ifndef BPT_H
#define BPT_H

#include <cstring>
#include "vector.h"
#include "utility.h"
#include <fstream>


struct id {
    int son_id;
    int my_id;

    id(int a, int b) : son_id(a), my_id(b) {}
};


template<typename Key, class N>
class bpt {
private:
    static const int M = 60;
    static const int L = 60;

    struct Leaf {
        std::pair <Key, N> unit[L + 2];
        int my_num;
        int next_num;
        int siz = 0;

        Leaf() : siz(0), my_num(1), next_num(0) {}

        Leaf(int siz_, int my_num_, int next_num_) : siz(siz_), my_num(my_num_), next_num(next_num_) {}
    };

    struct Node {
        int my_num, siz, son[M + 2] = {0}, son_is_leaf = 1;
        std::pair <Key, N> key[M + 2];

        Node() : my_num(1), siz(0) {}

        Node(int siz_, int num_, int l) : siz(siz_), my_num(num_), son_is_leaf(l) {}

//        Node(const Node &rhs):siz(rhs.siz),my_num(rhs.my_num),son_is_leaf(rhs.son_is_leaf){}
    };

    int leaf_num = 1;
    int leaf_1_index = 1;
    Leaf head_leaf;

    int node_num = 1;
    int node_1_index = 1;

    Node head_node;

    std::fstream file;
    std::fstream file1;
    const char *nod = new char;
    const char *lef = new char;

public:
    bpt(const char *node_file, const char *leaf_file) : nod(node_file), lef(leaf_file) {

        std::ifstream in1(node_file);
        if (!in1) {
            std::ofstream out(node_file);
            out.close();
            file.open(node_file);
            file.seekp(0);
            file.write(reinterpret_cast<char *>(&node_num), sizeof(int));
            file.write(reinterpret_cast<char *>(&node_1_index), sizeof(int));
            writeNode(head_node, head_node.my_num);
            file.close();
        }
        in1.close();
        std::ifstream in2(leaf_file);
        if (!in2) {
            std::ofstream out(leaf_file);
            out.close();
            file1.open(leaf_file);
            file1.seekp(0);
            file1.write(reinterpret_cast<char *>(&leaf_num), sizeof(int));
            file1.write(reinterpret_cast<char *>(&leaf_1_index), sizeof(int));
            writeLeaf(head_leaf, head_leaf.my_num);
            file1.close();
        }
        in2.close();

        file.open(nod);
        file1.open(lef);
        read_basic_leaf();
        read_basic_node();
        readNode(head_node, node_1_index);
        readLeaf(head_leaf, leaf_1_index);
    }

    ~bpt() {
        write_basic_node(node_num, head_node.my_num);
        write_basic_leaf(leaf_num, leaf_1_index);
    }

    void writeNode(const Node &a, int index) {
        file.seekp((index - 1) * sizeof(Node) + 2 * sizeof(int));
        file.write(reinterpret_cast<const char *>(&a), sizeof(Node));
        if (a.my_num == head_node.my_num) head_node = a;
    }

    void readNode(Node &a, int index) {
        file.seekg((index - 1) * sizeof(Node) + 2 * sizeof(int));
        file.read(reinterpret_cast<char *>(&a), sizeof(Node));
    }

    void writeLeaf(const Leaf &a, int index) {
        file1.seekp((index - 1) * sizeof(Leaf) + 2 * sizeof(int));
        file1.write(reinterpret_cast<const char *>(&a), sizeof(Leaf));
        if (a.my_num == head_leaf.my_num) head_leaf = a;
    }

    void readLeaf(Leaf &a, int index) {
        file1.seekg((index - 1) * sizeof(Leaf) + 2 * sizeof(int));
        file1.read(reinterpret_cast<char *>(&a), sizeof(Leaf));
    }

    void read_basic_node() {
        file.seekg(0);
        file.read(reinterpret_cast<char *>(&node_num), sizeof(int));
        file.read(reinterpret_cast<char *>(&node_1_index), sizeof(int));
    }

    void write_basic_node(int a, int b) {
        file.seekp(0);
        file.write(reinterpret_cast<const char *>(&a), sizeof(int));
        file.write(reinterpret_cast<const char *>(&b), sizeof(int));
    }

    void read_basic_leaf() {
        file1.seekg(0);
        file1.read(reinterpret_cast<char *>(&leaf_num), sizeof(int));
        file1.read(reinterpret_cast<char *>(&leaf_1_index), sizeof(int));
    }

    void write_basic_leaf(int a, int b) {
        file1.seekp(0);
        file1.write(reinterpret_cast<const char *>(&a), sizeof(int));
        file1.write(reinterpret_cast<const char *>(&b), sizeof(int));
    }

    void leaf_insert(Leaf &a,const  std::pair <Key, N> &tmp) {
        for (int i = 1; i <= a.siz; ++i) {
            //避免出现相同的键值对
            if (tmp == a.unit[i]) return;
            if (tmp < a.unit[i]) {
                for (int j = a.siz; j >= i; --j) {
                    a.unit[j + 1] = a.unit[j];
                }
                a.unit[i] = tmp;
                a.siz++;
                return;
            }
        }
        a.unit[++a.siz] = tmp;
    }


    void node_update1(Node &a, int node_i_, std::pair <Key, N> tmp, int son_) {//与后面的son对齐  +
        if (node_i_ > a.siz) {
            a.key[++a.siz] = tmp;
            a.son[a.siz] = son_;
        } else {
            for (int j = a.siz; j >= node_i_; --j) {
                a.key[j + 1] = a.key[j];
                a.son[j + 1] = a.son[j];
            }
            a.key[node_i_] = tmp;
            a.son[node_i_] = son_;
            a.siz++;
        }
    }

    void node_update4(Node &a, int node_i_, std::pair <Key, N> tmp, int son_) {//与前面的son对齐  +
        if (node_i_ > a.siz) {
            a.key[++a.siz] = tmp;
            a.son[a.siz] = son_;
        } else {
            a.son[a.siz + 1] = a.son[a.siz];
            for (int j = a.siz; j >= node_i_; --j) {
                a.key[j + 1] = a.key[j];
                a.son[j] = a.son[j - 1];
            }
            a.key[node_i_] = tmp;
            a.son[node_i_ - 1] = son_;
            a.siz++;
        }
    }


    vector<id> find_node(Node &tar, std::pair <Key, N> tmp) {//ans中记录的是son的下标
        vector<id> ans;
        while (!tar.son_is_leaf) {
            if (tmp < tar.key[1]) {
                int cur = tar.son[0];
                ans.push_back(id(0, tar.my_num));
                readNode(tar, cur);
            } else if (tmp >= tar.key[tar.siz]) {
                int cur = tar.son[tar.siz];
                ans.push_back(id(tar.siz, tar.my_num));
                readNode(tar, cur);
            } else {
                for (int i = 1; i < tar.siz; ++i) {
                    if (tar.key[i] <= tmp && tmp < tar.key[i + 1]) {
                        int cur = tar.son[i];
                        ans.push_back(id(i, tar.my_num));
                        readNode(tar, cur);
                        break;
                    }
                }
            }
        }
        return ans;
    }

    void find_leaf(Node &tar, Leaf &target_leaf, std::pair <Key, N> temp_leaf, int &node_i) {
        if (temp_leaf < tar.key[1]) {
            readLeaf(target_leaf, tar.son[0]);
            node_i = 0;
        } else {
            for (node_i = 1; node_i < tar.siz; ++node_i) {
                if (tar.key[node_i] <= temp_leaf && temp_leaf < tar.key[node_i + 1]) {
                    readLeaf(target_leaf, tar.son[node_i]);
                    return;
                }
            }
            node_i = tar.siz;
            readLeaf(target_leaf, tar.son[node_i]);
        }
    }

    void insert(const Key &index,const N &value) {
        vector<id> node_position;
        std::pair <Key, N> temp_leaf(index, value);

        if (head_node.siz == 0) {
            head_node.son[0] = head_leaf.my_num;
            leaf_insert(head_leaf, temp_leaf);
            //            head_node.key[1] = head_leaf.unit[head_leaf.siz];
            if (head_leaf.siz > L) {
                head_node.key[++head_node.siz] = head_leaf.unit[((L + 1) >> 1) + 1];
                head_node.son[head_node.siz] = (++leaf_num);
                head_leaf.siz = (L + 1) >> 1;
                head_leaf.next_num = leaf_num;
                Leaf tmp(L + 1 - head_leaf.siz, leaf_num, 0);
                for (int i = 1; i <= tmp.siz; ++i) {
                    tmp.unit[i] = head_leaf.unit[i + head_leaf.siz];
                }
                writeLeaf(tmp, tmp.my_num);
                writeNode(head_node, head_node.my_num);
            }
            writeLeaf(head_leaf, head_leaf.my_num);
        } else {

            //find the node
            Node tar = head_node;
            node_position = find_node(tar, temp_leaf);

            //find the leaf
            Leaf target_leaf = head_leaf;
            int node_i;
            find_leaf(tar, target_leaf, temp_leaf, node_i);

            //insert
            leaf_insert(target_leaf, temp_leaf);
            writeLeaf(target_leaf, target_leaf.my_num);

            //叶子节点超过L
            if (target_leaf.siz > L) {
                node_update1(tar, node_i + 1, target_leaf.unit[((L + 1) >> 1) + 1], ++leaf_num);
                target_leaf.siz = (L + 1) >> 1;
                Leaf tmp(L + 1 - target_leaf.siz, leaf_num, target_leaf.next_num);
                target_leaf.next_num = leaf_num;
                for (int i = 1; i <= tmp.siz; ++i) {
                    tmp.unit[i] = target_leaf.unit[i + target_leaf.siz];
                }
                writeLeaf(target_leaf, target_leaf.my_num);
                writeLeaf(tmp, tmp.my_num);
                writeNode(tar, tar.my_num);
                //索引节点超出M-1
                //节点与叶子不同在于：节点父亲key值不会在儿子中再出现
                while (tar.siz >= M - 1) {
                    Node par;
                    //处理父亲节点
                    if (!node_position.empty()) {
                        int tar_par = node_position.back().my_id;
                        readNode(par, tar_par);
                        int nodei = node_position.back().son_id + 1;
                        node_position.pop_back();
                        node_update1(par, nodei, tar.key[((M - 1) >> 1) + 1], ++node_num);
                    } else {//建立新的根节点
                        par.siz = 1;
                        par.son_is_leaf = 0;
                        par.key[1] = tar.key[((M - 1) >> 1) + 1];
                        par.son[0] = tar.my_num;
                        par.my_num = ++node_num;
                        node_1_index = node_num;
                        par.son[1] = ++node_num;
                        head_node = par;
                    }
                    //处理多出来的兄弟节点
                    Node temp_node(tar.siz - 1 - ((M - 1) >> 1), node_num, tar.son_is_leaf);
                    tar.siz = (M - 1) >> 1;
                    temp_node.son[0] = tar.son[tar.siz + 1];
                    for (int i = 1; i <= temp_node.siz; ++i) {
                        temp_node.key[i] = tar.key[i + tar.siz + 1];
                        temp_node.son[i] = tar.son[i + tar.siz + 1];
                    }
                    writeNode(temp_node, temp_node.my_num);
                    writeNode(par, par.my_num);
                    writeNode(tar, tar.my_num);
                    tar = par;
                }
            }
        }
        node_1_index = head_node.my_num;
    }


    int leaf_remove(Leaf &a, std::pair <Key, N> tmp) {
        for (int i = 1; i <= a.siz; ++i) {
            if (tmp == a.unit[i]) {
                for (int j = i; j < a.siz; ++j) {
                    a.unit[j] = a.unit[j + 1];
                }
                a.siz--;
                return i;
            }
        }
        return 0;
    }

    void node_update2(Node &a, int node_i_) {//后序--
        for (int j = node_i_; j < a.siz; ++j) {
            a.key[j] = a.key[j + 1];
            a.son[j] = a.son[j + 1];
        }
        a.siz--;
    }

    void node_update3(Node &a, int node_i_) {//前序--
        for (int j = node_i_; j < a.siz; ++j) {
            a.key[j] = a.key[j + 1];
            a.son[j - 1] = a.son[j];
        }
        a.son[a.siz - 1] = a.son[a.siz];
        a.siz--;
    }


    void remove(const Key &index, const N &value) {
        vector<id> ans;
        std::pair <Key, N> temp_leaf(index, value);

        //find the node
        Node tar = head_node;
        ans = find_node(tar, temp_leaf);

        //find the leaf
        if (head_leaf.next_num == 0) {
            leaf_remove(head_leaf, temp_leaf);
            writeLeaf(head_leaf, head_leaf.my_num);
            return;
        }
        Leaf target_leaf;
        int node_i = 0;
        find_leaf(tar, target_leaf, temp_leaf, node_i);

        //排除特殊情况——删除叶节点最上面的unit或者索引节点第0个儿子 索引节点的key需要更改
        int erase_leaf_res = leaf_remove(target_leaf, temp_leaf);
        if (!erase_leaf_res) {
            return;
        }
        if (erase_leaf_res == 1) {
            if (node_i != 0)
                tar.key[node_i] = target_leaf.unit[1];
            else {
                Node nd;
                vector<id> answer = ans;
                while (!answer.empty()) {
                    int cur_id = answer.back().my_id;
                    int ptr = answer.back().son_id;
                    answer.pop_back();
                    if (ptr) {
                        readNode(nd, cur_id);
                        nd.key[ptr] = target_leaf.unit[1];
                        writeNode(nd, nd.my_num);
                        break;
                    }
                }
            }
        }

        //if the leaf is thin
        if (target_leaf.siz < (L >> 1) && target_leaf.siz > 0) {
            bool flag = false;
            Leaf bro;
            //borrow an element from next
            if (node_i < tar.siz) {
                readLeaf(bro, target_leaf.next_num);
                if (bro.siz > (L >> 1)) {
                    target_leaf.unit[++target_leaf.siz] = bro.unit[1];
                    bro.siz--;
                    for (int i = 1; i <= bro.siz; ++i) {
                        bro.unit[i] = bro.unit[i + 1];
                    }
                    tar.key[node_i + 1] = bro.unit[1];
                    flag = true;
                }
            }
            if (!flag && node_i >= 1) {
                //borrow an element from prev
                readLeaf(bro, tar.son[node_i - 1]);
                if (bro.siz > (L >> 1)) {
                    ++target_leaf.siz;
                    for (int i = target_leaf.siz; i >= 1; --i) {
                        target_leaf.unit[i + 1] = target_leaf.unit[i];
                    }
                    target_leaf.unit[1] = bro.unit[bro.siz];
                    bro.siz--;
                    tar.key[node_i] = target_leaf.unit[1];
                    flag = true;
                }
            }
            if (flag)
                writeLeaf(bro, bro.my_num);
            if (!flag) {
                Node par;
                Node brother;
                //merge
                if (node_i != tar.siz) {
                    readLeaf(bro, target_leaf.next_num);
                    for (int i = target_leaf.siz + 1, j = 1; j <= bro.siz; ++i, ++j) {
                        target_leaf.unit[i] = bro.unit[j];
                    }
                    target_leaf.siz += bro.siz;
                    target_leaf.next_num = bro.next_num;
                    node_update2(tar, node_i + 1);
                } else {
                    readLeaf(bro, tar.son[node_i - 1]);
                    for (int i = bro.siz + 1, j = 1; j <= target_leaf.siz; ++i, ++j) {
                        bro.unit[i] = target_leaf.unit[j];
                    }
                    bro.siz += target_leaf.siz;
                    bro.next_num = target_leaf.next_num;
                    node_update2(tar, node_i);
                }
                writeLeaf(bro, bro.my_num);
                //judge the finding node to see if they need to be merged
                while (tar.siz > 0 && tar.siz <= (M >> 1) - 1) {

                    if (!ans.empty()) {
                        int par_id = ans.back().my_id;
                        readNode(par, par_id);
                    } else
                        break;

                    int nodei = ans.back().son_id;
                    ans.pop_back();
                    bool isleft = true;
                    bool success = false;
                    //向兄弟节点借
                    if (nodei >= 1) {
                        readNode(brother, par.son[nodei - 1]);
                        if (brother.siz > (M >> 1)) {
                            std::pair <Key, N> temp = brother.key[brother.siz];
                            node_update4(tar, 1, par.key[nodei], brother.son[brother.siz]);
                            node_update2(brother, brother.siz);
                            par.key[nodei] = temp;
                            success = true;
                        }
                    }

                    if (!success && nodei < par.siz) {
                        readNode(brother, par.son[nodei + 1]);
                        isleft = false;
                        if (brother.siz > (M >> 1)) {
                            std::pair <Key, N> temp = brother.key[1];
                            node_update1(tar, tar.siz + 1, par.key[nodei + 1], brother.son[0]);
                            node_update3(brother, 1);
                            par.key[nodei + 1] = temp;
                            success = true;
                        }
                    }
                    //merge
                    if (!success) {
                        if (!isleft) {
                            tar.key[++tar.siz] = par.key[nodei + 1];
                            tar.son[tar.siz] = brother.son[0];
                            for (int i = 1, j = tar.siz + 1; i <= brother.siz; ++i, ++j) {
                                node_update4(tar, j, brother.key[i], brother.son[i]);
                            }
                            //                        tar.son[tar.siz] = brother.son[brother.siz];
                            //                        if (!tar.son_is_leaf) change_node_par(tar.son[tar.siz], tar.my_num);
                            node_update2(par, nodei + 1);
                            if (par.siz == 0 && ans.empty()) {
                                head_node = tar;
                                node_1_index = tar.my_num;
                                //                                break;
                            }
                        } else {
                            brother.key[++brother.siz] = par.key[nodei];
                            brother.son[brother.siz] = tar.son[0];
                            for (int i = 1, j = brother.siz + 1; i <= tar.siz; ++i, ++j) {
                                node_update4(brother, j, tar.key[i], tar.son[i]);
                            }
                            node_update2(par, nodei);
                            if (par.siz == 0 && ans.empty()) {
                                head_node = brother;
                                node_1_index = brother.my_num;
                            }
                        }
                    }
                    writeNode(par, par.my_num);
                    writeNode(brother, brother.my_num);
                    writeNode(tar, tar.my_num);
                    tar = par;
                }
            }
        }
        writeLeaf(target_leaf, target_leaf.my_num);
        writeNode(tar, tar.my_num);
    }


    vector<N> find(const Key &key) {
        vector<N> ans;
        Node tar = head_node;

        // find node
        while (!tar.son_is_leaf) {
            if (key <= tar.key[1].first) {
                int cur = tar.son[0];
                readNode(tar, cur);
            } else {
                bool ok = false;
                for (int i = 1; i < tar.siz; ++i) {
                    if (tar.key[i].first <= key && key <= tar.key[i + 1].first) {
                        int cur = tar.son[i];
                        readNode(tar, cur);
                        ok = true;
                        break;
                    }
                }
                if (!ok) readNode(tar, tar.son[tar.siz]);
            }
        }
        Leaf target_leaf = head_leaf;

        //find leaf
        if (leaf_num > 1) {
            if (key <= tar.key[1].first) {
                readLeaf(target_leaf, tar.son[0]);
            } else {
                bool ok = false;
                for (int node_i = 1; node_i < tar.siz; ++node_i) {
                    if (tar.key[node_i].first <= key && key <= tar.key[node_i + 1].first) {
                        readLeaf(target_leaf, tar.son[node_i]);
                        ok = true;
                        break;
                    }
                }
                if (!ok) readLeaf(target_leaf, tar.son[tar.siz]);
            }
        }
        while (target_leaf.my_num) {
            bool equal = true;
            for (int i = 1; i <= target_leaf.siz; ++i) {
                if (key == target_leaf.unit[i].first) {
                    ans.push_back(target_leaf.unit[i].second);
                }
                if (key < target_leaf.unit[i].first) {
                    equal = false;
                    break;
                }
            }
            if (!equal) break;
            int num = target_leaf.next_num;
            if (num == 0) break;
            readLeaf(target_leaf, num);
        }
        return ans;
    }


    void clean() {
        Node a;
        head_node=a;
        Leaf b;
        head_leaf=b;
        node_num = node_1_index = 1;
        leaf_num = leaf_1_index = 1;
        writeNode(head_node, head_node.my_num);
        writeLeaf(head_leaf,head_leaf.my_num);
    }

//    void modify(const Key &key,const N &value){
//        Node tar = head_node;
//
//        // find node
//        while (!tar.son_is_leaf) {
//            if (key <= tar.key[1].first) {
//                int cur = tar.son[0];
//                readNode(tar, cur);
//            } else {
//                bool ok = false;
//                for (int i = 1; i < tar.siz; ++i) {
//                    if (tar.key[i].first <= key && key <= tar.key[i + 1].first) {
//                        int cur = tar.son[i];
//                        readNode(tar, cur);
//                        ok = true;
//                        break;
//                    }
//                }
//                if (!ok) readNode(tar, tar.son[tar.siz]);
//            }
//        }
//        Leaf target_leaf = head_leaf;
//
//        //find leaf
//        if (leaf_num > 1) {
//            if (key <= tar.key[1].first) {
//                readLeaf(target_leaf, tar.son[0]);
//            } else {
//                bool ok = false;
//                for (int node_i = 1; node_i < tar.siz; ++node_i) {
//                    if (tar.key[node_i].first <= key && key <= tar.key[node_i + 1].first) {
//                        readLeaf(target_leaf, tar.son[node_i]);
//                        ok = true;
//                        break;
//                    }
//                }
//                if (!ok) readLeaf(target_leaf, tar.son[tar.siz]);
//            }
//        }
//
//        for (int i = 1; i <= target_leaf.siz; ++i) {
//            if (key == target_leaf.unit[i].first ) {
//                target_leaf.unit[i].second = value;
//                writeLeaf(target_leaf,target_leaf.my_num);
//            }
//        }
//
//        if(leaf_num==1) head_leaf=target_leaf;

//    }
};

#endif