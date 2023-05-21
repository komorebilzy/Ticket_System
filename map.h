/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "exception.h"
#include "utility.h"


//实现了rbegin和rend 及其对应的++ --操作
//简单写了一下upper_bound和lower_bound 可能不太对


    //1.定义一个 type_traits 模板类
    template<typename T>
    struct my_type_traits{
        using iterator_assignable = typename T::iterator_assignable;
    };
//2.定义两个类，表示迭代器是否可被赋值的特性(这一步也可以使用枚举变量来实现)
    struct my_true_type{
        static const bool isassignable=true;
    };
    struct my_false_type{
        static const bool isassignable=false;
    };
//3.分别在可被赋值的迭代器和不可被赋值的迭代器中定义 iterator_assignable 类型


    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {

    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use std::map as value_type by typedef.
         */
        typedef std::pair<const Key, T> value_type;


        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */

    private:
        struct node {
            value_type data;
            node *left = nullptr;
            node *right = nullptr;
            node *parent = nullptr;    //利于实现iterator++ --
            int height;

            node(const value_type &data_, int h_, node *l_, node *r_, node *p_) : data(data_), height(h_), left(l_),
                                                                                  right(r_),
                                                                                  parent(p_) {}

            node(int h_, node *l_, node *r_, node *p_) : height(h_), left(l_), right(r_), parent(p_) {}

            node(const node &a) : data(a.data), left(a.left), right(a.right), parent(a.parent), height(a.height) {}
        };

        node *flag = nullptr;    //用于insert 记录当前节点t=nullptr的父节点
        node *root = nullptr;
        size_t num = 0;
    public:
        class const_iterator;

        class iterator {
        public:
            using iterator_assignable = my_true_type;

        public:
            node *pos;
            const map<Key, T, Compare> *target;

            iterator(node *pos_ = nullptr, const map<Key, T, Compare> *tar_ = nullptr,bool state_=true) : pos(pos_), target(tar_),state(state_){}

            iterator(const iterator &other) {
                pos = other.pos;
                target = other.target;
                state=other.state;
            }
            bool state =true;

            /**
             *  ++iter
             */
            iterator &operator++() {           //关键点在于右子树是否为空
                if (state) {
                    if (pos == nullptr) throw invalid_iterator();
                    if (pos->right) {
                        node *lef = pos->right;                    //非空 找右子树的最左节点
                        while (lef->left) lef = lef->left;
                        pos = lef;
                    } else {
                        node *par = pos->parent;               //为空 找右子树不在当前树枝的父节点
                        node *cur = pos;
                        while (cur == par->right) {
                            cur = cur->parent;
                            par = par->parent;
                            if (par == nullptr) break;
                        }
                        pos = par;
                    }
                    return *this;
                }
                else return --(*this);
            }


            /**
             *  iter++
             */
            iterator operator++(int) {
                iterator tmp(*this);
                ++(*this);
                return tmp;
            }

            /**
             *  --iter
             */


            iterator least() {
                node *cur = target->root;
                while (cur != nullptr && cur->left) cur = cur->left;
                return iterator(cur, target);
            }

            iterator &operator--() {
                if(state) {
                    if (*this == least()) throw invalid_iterator();
                    if (pos == nullptr) {
                        pos = target->root;
                        while (pos->right) pos = pos->right;
                    } else if (pos->left) {
                        node *rig = pos->left;
                        while (rig->right) rig = rig->right;
                        pos = rig;
                    } else {
                        node *par = pos->parent;
                        node *cur = pos;
                        while (cur == par->left) {
                            cur = cur->parent;
                            par = par->parent;
                            if (par == nullptr) break;
                        }
                        pos = par;
                    }
                    return *this;
                }
                else return ++(*this);
            }

            /**
             *  iter--
             */
            iterator operator--(int) {
                iterator tmp(*this);
                --(*this);
                return tmp;
            }


            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return pos->data;
            }

            bool operator==(const iterator &rhs) const {
                return (pos == rhs.pos) && (target == rhs.target);
            }

            bool operator==(const const_iterator &rhs) const {
                return (pos == rhs.pos) && (target == rhs.target);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !operator==(rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !operator==(rhs);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                value_type *tmp = &(pos->data);
                return tmp;
            }
        };

        class const_iterator {
        public:
            using iterator_assignable = my_false_type;
            // it should have similar member method as iterator.
            //  and it should be able to construct from an iterator.
        public:
            const map<Key, T, Compare> *target;
            const node *pos;

            const_iterator(const node *pos_ = nullptr, const map<Key, T, Compare> *tar_ = nullptr) : pos(pos_),
                                                                                                     target(tar_) {}

            const_iterator(const const_iterator &other) {
                target = other.target;
                pos = other.pos;
            }

            const_iterator(const iterator &other) {
                target = other.target;
                pos = other.pos;
            }

            const_iterator &operator++() {
                if (pos == nullptr) throw invalid_iterator();
                if (pos->right) {
                    const node *lef = pos->right;
                    while (lef->left) lef = lef->left;
                    pos = lef;
                } else {
                    const node *par = pos->parent;
                    const node *cur = pos;
                    while (cur == par->right) {
                        cur = cur->parent;
                        par = par->parent;
                        if (par == nullptr) break;
                    }
                    pos = par;
                }
                return *this;
            }


            const_iterator operator++(int) {
                const_iterator tmp(*this);
                ++(*this);
                return tmp;
            }


            const_iterator least() {
                const node *cur = target->root;
                while (cur != nullptr && cur->left != nullptr) cur = cur->left;
                return const_iterator(cur, target);
            }

            const_iterator &operator--() {
                if (*this == least()) throw invalid_iterator{};
                if (pos == nullptr) {
                    pos = target->root;
                    while (pos->right) pos = pos->right;
                } else if (pos->left) {
                    node *rig = pos->left;
                    while (rig->right) rig = rig->right;
                    pos = rig;
                } else {
                    const node *par = pos->parent;
                    const node *cur = pos;
                    while (cur == par->left) {
                        cur = cur->parent;
                        par = par->parent;
                        if (par == nullptr) break;
                    }
                    pos = par;
                }
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp(*this);
                --(*this);
                return tmp;
            }

            const value_type &operator*() const {
                return pos->data;
            }

            bool operator==(const iterator &rhs) const {
                return (pos == rhs.pos) && (target == rhs.target);
            }

            bool operator==(const const_iterator &rhs) const {
                return (pos == rhs.pos) && (target == rhs.target);
            }

            bool operator!=(const iterator &rhs) const {
                return !operator==(rhs);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !operator==(rhs);
            }

            const value_type *operator->() const noexcept {
                return &(pos->data);
            }


        };


        map() { root = nullptr; }

        map(const map &other) { this->operator=(other); }


        node *CopyTree(node *t) {
            if (!t) return nullptr;
            node *newL = nullptr;
            node *newR = nullptr;
            if (t->left) newL = CopyTree(t->left);
            if (t->right) newR = CopyTree(t->right);
            node *newTree = new node(t->data, t->height, newL, newR, nullptr);
            if (newL != nullptr) newL->parent = newTree;
            if (newR != nullptr) newR->parent = newTree;
            return newTree;
        }


        map &operator=(const map &other) {
            if (this == &other) return *this;
            clear();
            num = other.num;
            root = CopyTree(other.root);
            return *this;
        }


        ~map() {
            clear();
        }

        /**
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */

        //区别：用于不同的map
        //带const用于const map 返回不可改变的值
        //而普通的at可以用于正常的map 返回的值可以被改变


        T &at(const Key &key) {
            node *t = root;
            while (t != nullptr && (Compare()(t->data.first, key) || Compare()(key, t->data.first))) {
                if (Compare()(key, t->data.first)) t = t->left;
                else t = t->right;
            }
            if (t == nullptr) throw index_out_of_bound{};
            return t->data.second;
//            return const_cast<T&>(const_cast<const map *>(this)->at(key));
        }

        const T &at(const Key &key) const {
            node *t = root;
            while (t != nullptr && (Compare()(t->data.first, key) || Compare()(key, t->data.first))) {
                if (Compare()(key, t->data.first)) t = t->left;
                else t = t->right;
            }
            if (t == nullptr) throw index_out_of_bound{};
            return t->data.second;
        }

        /**
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            return insert({key, {}}).first->second;
//            node *t = root;
//            while (t != nullptr && (Compare()(t->data.first, key) || Compare()(key, t->data.first))) {
//                if (Compare()(key, t->data.first)) t = t->left;
//                else t = t->right;
//            }
//            if (t == nullptr) {
//                //error:value_type a; 没有默认构造函数
//                return insert({key, {}}).first->second;
//            }
//            return t->data.second;
        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {    //想调用at函数 但是不知道怎么显示调用const函数
            node *t = root;
            while (t != nullptr && (Compare()(t->data.first, key) || Compare()(key, t->data.first))) {
                if (Compare()(key, t->data.first)) t = t->left;
                else t = t->right;
            }
            if (t == nullptr) throw index_out_of_bound{};
            return t->data.second;
        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            node *l = root;
            while (l && l->left) {
                l = l->left;
            }
            return iterator(l, this);
        }

        const_iterator cbegin() const {
            node *l = root;
            while (l && l->left) {
                l = l->left;
            }
            return const_iterator(l, this);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() const {
            return iterator(nullptr, this);
        }

        const_iterator cend() const {
            return const_iterator(nullptr, this);
        }



        iterator rbegin() const {
            node *r = root;
            while (r && r->right) {
                r = r->right;
            }
            return iterator(r, this,false);
        }

        iterator rend()const {
            return iterator(nullptr,this,false);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return num == 0;
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return num;
        }

        /**
         * clears the contents
         */
        void clear1(node *&t) {
            if (t == nullptr) return;
            clear1(t->left);
            clear1(t->right);
            delete (t);
            t = nullptr;
        }

        void clear() {
            clear1(root);
            num = 0;
            flag = nullptr;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        int max(int a, int b) {
            if (a > b) return a;
            else return b;
        }

        int high(node *t) {
            if (t == nullptr) return 0;     //很重要！！！
            else return t->height;
        }

        void LL(node *&t) {
            node *t1 = t->left;
            t->left = t1->right;
            if (t1->right) t1->right->parent = t;
            t1->parent = t->parent;
            t->parent = t1;
            t1->right = t;
            t->height = max(high(t->left), high(t->right)) + 1;
            t1->height = max(high(t1->left), high(t)) + 1;
            if (t1->parent) {
                if (t1->parent->left == t) t1->parent->left = t1;
                else t1->parent->right = t1;
//                printf("&left=%llx &t=%llx t=%llx\n", &t1->parent->left, &t, t);
            }
            t = t1;
        }

        void RR(node *&t) {
            node *t1 = t->right;
            t->right = t1->left;
            if (t1->left) t1->left->parent = t;
            t1->parent = t->parent;
            t->parent = t1;
            t1->left = t;
            t->height = max(high(t->left), high(t->right)) + 1;
            t1->height = max(high(t1->right), high(t)) + 1;
            if (t1->parent) {
                if (t1->parent->left == t) t1->parent->left = t1;
                else t1->parent->right = t1;
            }
            t = t1;
        }

        void LR(node *&t) {
            RR(t->left);
            LL(t);
        }

        void RL(node *&t) {
            LL(t->right);
            RR(t);
        }

        std::pair<iterator, bool> insert1(const value_type &value, node *&t) {
            if (t == nullptr) {
                t = new node(value, 1, nullptr, nullptr, flag);
                iterator m(t, this);
                return std::pair<iterator, bool>(m, true);

            } else if (Compare()(value.first, t->data.first)) {
                flag = t;
                std::pair<iterator, bool> tmp(insert1(value, t->left));
                if (tmp.second && high(t->left) - high(t->right) == 2) {
                    if (Compare()(value.first, t->left->data.first)) LL(t);
                    else LR(t);
                }
                t->height = max(high(t->left), high(t->right)) + 1;
                return tmp;
            } else if (Compare()(t->data.first, value.first)) {
                flag = t;
                std::pair<iterator, bool> tmp(insert1(value, t->right));
                if (tmp.second && high(t->right) - high(t->left) == 2) {
                    if (Compare()(t->right->data.first, value.first)) RR(t);
                    else RL(t);
                }
                t->height = max(high(t->left), high(t->right)) + 1;
                return tmp;
            } else {                             //对应节点已经存在
                iterator m(t, this);
                return std::pair<iterator, bool>(m, false);
            }

        }


        std::pair<iterator, bool> insert(const value_type &value) {
            flag = nullptr;
            std::pair<iterator, bool> tmp(insert1(value, root));
            if (tmp.second) num++;
            return tmp;
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        bool adjust(node *&t, int SubTree) {
            if (SubTree) {
                if (high(t->left) - high(t->right) == 1) return true;
                if (high(t->right) == high(t->left)) {
                    --t->height;
                    return false;
                }
                if (high(t->left->right) > high(t->left->left)) {
                    LR(t);
                    return false;
                }
                LL(t);
                if (high(t->right) == high(t->left)) return false;
                else return true;
            } else {
                if (high(t->right) - high(t->left) == 1) return true;
                if (high(t->right) == high(t->left)) {
                    --t->height;
                    return false;
                }
                if (high(t->right->left) > high(t->right->right)) {
                    RL(t);
                    return false;
                }
                RR(t);
                if (high(t->right) == high(t->left)) return false;
                else return true;
            }
        }

        bool remove(const Key &x, node *&t) {
            if (t == nullptr) return true;
            if (Compare()(x, t->data.first)) {
                if (remove(x, t->left)) return true;
                return adjust(t, 0);
            } else if (Compare()(t->data.first, x)) {
                if (remove(x, t->right)) return true;
                return adjust(t, 1);
            } else {
                if (t->left == nullptr || t->right == nullptr) {
                    node *oldnode = t;
                    t = (t->left != nullptr) ? t->left : t->right;
                    if (t != nullptr) t->parent = oldnode->parent;
                    delete (oldnode);
                    return false;
                } else {
                    node *tmp = t->right;
                    while (tmp->left != nullptr) tmp = tmp->left;
                    int temp = tmp->height;
                    tmp->height = t->height;
                    t->height = temp;
                    //swap two node* 替身交换
                    if (tmp == t->right) {      //特殊情况 单独处理
                        node *pa = t->parent;
                        node *son = t->left;
                        node *son1 = tmp->right;

                        t->parent = tmp;
                        t->left = nullptr;
                        t->right = son1;
                        tmp->parent = pa;
                        tmp->left = son;
                        tmp->right = t;

                        if (pa != nullptr) {
                            if (pa->left == t) pa->left = tmp;
                            else pa->right = tmp;
                        }
                        if (son != nullptr) son->parent = tmp;
                        if (son1 != nullptr) son1->parent = t;
                        t = tmp;

                    } else {
                        node *pa = tmp->parent;
                        node *son = tmp->right;
                        node *pa1 = t->parent;
                        node *son1 = t->left;
                        node *son2 = t->right;

                        tmp->parent = pa1;
                        tmp->left = son1;
                        tmp->right = son2;
                        t->left = nullptr;
                        t->right = son;
                        t->parent = pa;
                        if (son != nullptr) {
                            son->parent = t;
                        }
                        pa->left = t;
                        if (son1 != nullptr) son1->parent = tmp;
                        son2->parent = tmp;
                        if (pa1 != nullptr) {
                            if (t == pa1->left) pa1->left = tmp;
                            else pa1->right = tmp;
                        }
                        t = tmp;
                    }
                    //

                    if (remove(x, t->right)) return true;
                    return adjust(t, 1);
                }
            }
        }


        void erase(iterator pos_) {
            node *tmp = pos_.pos;
            //注意一些特殊情况：指针为空 找不到 指向的不是同一个vector
            if (tmp == nullptr || !count(tmp->data.first) || pos_.target != this) throw invalid_iterator();
            num--;
            remove(tmp->data.first, root);
        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        bool count1(const Key &key, node *t) const {
            if (t == nullptr) return false;
            if (Compare()(t->data.first, key)) return count1(key, t->right);
            if (Compare()(key, t->data.first)) return count1(key, t->left);
            return true;
        }


        size_t count(const Key &key) const {
            if (count1(key, root)) return 1;
            else return 0;
        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find1(const Key &key, node *t) {
            if (t == nullptr) return end();
            if (Compare()(key, t->data.first)) return find1(key, t->left);
            else if (Compare()(t->data.first, key)) return find1(key, t->right);
            else return iterator(t, this);
        }

        const_iterator find2(const Key &key, node *t) const {
            if (t == nullptr) return end();
            if (Compare()(key, t->data.first)) return find2(key, t->left);
            else if (Compare()(t->data.first, key)) return find2(key, t->right);
            else return const_iterator(t, this);
        }

        iterator find(const Key &key) {
            return find1(key, root);
        }

        const_iterator find(const Key &key) const {
            return find2(key, root);
        }



        iterator upper_bound (const Key& k){
            iterator tmp= find(k);
            ++tmp;
            return tmp;
        }
        const_iterator upper_bound (const Key& k) const{
            const_iterator tmp=find(k);
            ++tmp;
            return tmp;
        }
        iterator lower_bound (const Key& k){
            iterator tmp=find(k);
            ++tmp;
            return tmp;
        }
        const_iterator lower_bound (const Key& k) const{
            const_iterator tmp= find(k);
            ++tmp;
            return tmp;
        }


    };


#endif
