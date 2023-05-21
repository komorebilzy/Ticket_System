#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exception.h"
#include <climits>
#include <cstddef>

//要点：
//1.在iterator类中传入vector指针，代表其对应的具体的vector
//2.使用（T*）malloc(sizeof(T)*siz)分配整块空间
//使用new()T()进行构造   insert
//使用.~T（）删除空间    erase
//使用free归还全部空间
//3.不要忘记赋值函数的特判==*this

//满意的点
//iterator的构造函数写的不错，靠自己把思路理顺了qaq



/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
    template<typename T>
    class vector {
    public:
        int siz;
        int current_siz;
        T *storage;

        /**
         *
         * a type for actions of the elements of a vector, and you should write
         *   a class named const_iterator with same interfaces.
         */
        /**
         * you can see RandomAccessIterator at CppReference for help.
         */
        class const_iterator;

        class iterator {
            // The following code is written for the C++ type_traits library.
            // Type traits is a C++ feature for describing certain properties of a type.
            // For instance, for an iterator, iterator::value_type is the type that the
            // iterator points to.
            // STL algorithms and containers may use these type_traits (e.g. the following
            // typedef) to work properly. In particular, without the following code,
            // @code{std::sort(iter, iter1);} would not compile.
            // See these websites for more information:
            // https://en.cppreference.com/w/cpp/header/type_traits
            // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
            // About iterator_category: https://en.cppreference.com/w/cpp/iterator
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            int pos;
            vector<value_type> *temp;
        public:
            iterator(vector<value_type> *temp_ = nullptr, int pos_ = -1) : pos(pos_) {
                temp = temp_;
            }

            iterator(const iterator &other) : pos(other.pos) {
                temp = other.temp;
            }

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */

            iterator operator+(const int &n) const {
                iterator tmp(temp, pos + n);
                if (tmp.pos > temp->current_siz) throw runtime_error();
                return tmp;
            }

            iterator operator-(const int &n) const {
                iterator tmp(temp, pos + n);
                if (tmp.pos < 0) throw runtime_error();
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (temp != rhs.temp) throw invalid_iterator();
                return this->pos - rhs->pos;
            }

            iterator &operator+=(const int &n) {
                pos += n;
                if (pos > temp->current_siz) throw runtime_error();
                return *this;
            }

            iterator &operator-=(const int &n) {
                pos -= n;
                if (pos < 0) throw runtime_error();
                return *this;
            }

            iterator operator++(int) {
                iterator tmp(temp, ++pos);
                if (pos > temp->current_siz) throw runtime_error();
                return tmp;
            }

            iterator &operator++() {
                pos += 1;
                if (pos > temp->current_siz) throw runtime_error();
                return *this;
            }

            iterator operator--(int) {
                iterator tmp(temp, --pos);
                if (pos < 0) throw runtime_error();
                return tmp;
            }

            iterator &operator--() {
                pos -= 1;
                if (pos < 0) throw runtime_error();
                return *this;
            }

            T &operator*() const {
                return temp->storage[pos];
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             * you should mind that the iterators and const_iterator are the same if they are |^
             */
            bool operator==(const iterator &rhs) const {
                return (temp == rhs.temp) && (pos == rhs.pos);
            }

            bool operator==(const const_iterator &rhs) const {
                return (temp == rhs.temp) && (pos == rhs.pos);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(operator==(rhs));
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(operator==(rhs));
            }

            friend class vector<value_type>;
        };

        /**
         * has same function as iterator, just for a const object.
         */
        class const_iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T *;
            using reference = T &;
            using iterator_category = std::output_iterator_tag;

        private:
            int pos;
            const vector<value_type> *temp;

        public:
            const_iterator(const vector<value_type> *temp_ = nullptr, int pos_ = -1) : temp(temp_), pos(pos_) {}

            const_iterator(const const_iterator &other) : pos(other.pos), temp(other.temp) {}

            /**
             * return a new iterator which pointer n-next elements
             * as well as operator-
             */

            const_iterator operator+(const int &n) const {
                const_iterator tmp(temp, pos + n);
                if (tmp.pos > temp->current_siz) throw runtime_error();
                return tmp;
            }

            const_iterator operator-(const int &n) const {
                const_iterator tmp(temp, pos - n);
                if (tmp.pos < 0) throw runtime_error();
                return tmp;
            }

            // return the distance between two iterators,
            // if these two iterators point to different vectors, throw invaild_iterator.
            int operator-(const const_iterator &rhs) const {
                if (temp != rhs.temp) throw invalid_iterator();
                else return this->pos - rhs->pos;
            }

            const_iterator &operator+=(const int &n) {
                pos += n;
                if (pos > temp->current_siz) throw runtime_error();
                return *this;
            }

            const_iterator &operator-=(const int &n) {
                pos -= n;
                if (pos < 0) throw runtime_error();
                return *this;
            }


            const_iterator operator++(int) {
                const_iterator tmp(temp, ++pos);
                if (pos > temp->current_siz) throw runtime_error();
                return tmp;
            }


            const_iterator &operator++() {
                pos += 1;
                if (pos > temp->current_siz) throw runtime_error{};
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator tmp(temp, --pos);
                if (pos < 0) throw runtime_error{};
                return tmp;
            }

            const_iterator &operator--() {
                pos -= 1;
                if (pos < 0) throw runtime_error{};
                return *this;
            }

            const T &operator*() const {
                return temp->storage[pos];
            }

            /**
             * a operator to check whether two iterators are same (pointing to the same memory address).
             */
            bool operator==(const iterator &rhs) const {
                return (temp == rhs.temp) && (pos == rhs.pos);
            }

            bool operator==(const const_iterator &rhs) const {
                return (temp == rhs.temp) && (pos == rhs.pos);
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(operator==(rhs));
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(operator==(rhs));
            }

            friend class vector<value_type>;

        };

        /**
         * Constructs
         * At least two: default constructor, copy constructor
         */
        vector(int size_ = 0) : siz(size_) {
            if (siz == 0) storage = nullptr;
            else storage = (T *) malloc(sizeof(T) * siz);
            current_siz = siz;
            for (int i = 0; i < current_siz; ++i) {
                new(storage + i)T(storage[i]);
            }
        }

        vector(const vector &other) {
            siz = other.siz;
            current_siz = other.current_siz;
            storage = (T *) malloc(sizeof(T) * siz);
            for (int i = 0; i < current_siz; ++i) {
                new(storage + i)T(other.storage[i]);
            }
        }

        /**
         * Destructor
         */
        ~vector() {
            for (int i = 0; i < current_siz; ++i) {
                storage[i].~T();
            }
            free(storage);
            storage = nullptr;
            siz = 0;
        }


        vector &operator=(const vector &other) {
            if (this == &other) return *this;
            this->~vector();
            new(this)vector(other);       //巧妙的代码复用
            return *this;
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         */
        T &at(const size_t &pos) {
            if (pos < 0 || pos >= current_siz) throw index_out_of_bound();
            return (*this)[pos];
        }

        const T &at(const size_t &pos) const {
            if (pos < 0 || pos >= current_siz) throw index_out_of_bound();
            return (*this)[pos];
        }

        /**
         * assigns specified element with bounds checking
         * throw index_out_of_bound if pos is not in [0, size)
         * !!! Pay attentions
         *   In STL this operator does not check the boundary but I want you to do.
         */
        T &operator[](const size_t &pos) {
            if (pos < 0 || pos >= current_siz) throw index_out_of_bound();
            return storage[pos];
        }

        const T &operator[](const size_t &pos) const {
            if (pos < 0 || pos >= current_siz) throw index_out_of_bound();
            return storage[pos];
        }

        /**
         * access the first element.
         * throw container_is_empty if size == 0
         */
        const T &front() const {
            if (current_siz == 0) throw container_is_empty();
            return storage[0];
        }

        /**
         * access the last element.
         * throw container_is_empty if size == 0
         */
        const T &back() const {
            if (current_siz == 0) throw container_is_empty();
            return storage[current_siz - 1];
        }

        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            iterator m(this, 0);
            return m;
        }

        const_iterator cbegin() const {
            const_iterator m(this, 0);
            return m;
        }

        /**
         * returns an iterator to the end.
         */
        iterator end() {
            iterator m(this, current_siz);
            return m;
        }

        const_iterator cend() const {
            const_iterator m(this, current_siz);
            return m;
        }

        /**
         * checks whether the container is empty
         */
        bool empty() const {
            return current_siz == 0;
        }

        /**
         * returns the number of elements
         */
        size_t size() const {
            return current_siz;
        }

        /**
         * clears the contents
         */
        void clear() {
            current_siz = 0;
        }

        void resize() {
            if (siz == 0) {
                siz = 1;
            }
            T *tmp = storage;
            siz *= 2;
            storage = (T *) malloc(sizeof(T) * siz);
            for (int i = 0; i < current_siz; ++i) {
                new(storage + i)T(std::move(tmp[i]));          //by kupi
                tmp[i].~T();
            }
            free(tmp);
        }
//    void minus_size(){
//        T *tmp=storage;
//        siz/=2;
//
//    }
        /**
         * inserts value before pos
         * returns an iterator pointing to the inserted value.
         */
        iterator insert(iterator pos, const T &value) {
            if (current_siz == siz) resize();
            for (int i = current_siz - 1; i >= pos.pos; --i) {
                storage[i + 1] = storage[i];
            }
            storage[pos.pos] = value;
            ++current_siz;
            return pos;

        }

        /**
         * inserts value at index ind.
         * after inserting, this->at(ind) == value
         * returns an iterator pointing to the inserted value.
         * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
         */
        iterator insert(const size_t &ind, const T &value) {
            if (ind > current_siz) throw index_out_of_bound();
            if (current_siz == siz) resize();
            for (int i = current_siz - 1; i >= ind; --i) {
                storage[i + 1] = storage[i];
            }
            storage[ind] = value;
            ++current_siz;
            return iterator(this, ind);
        }

        /**
         * removes the element at pos.
         * return an iterator pointing to the following element.
         * If the iterator pos refers the last element, the end() iterator is returned.
         */
        iterator erase(iterator pos) {
            for (int i = pos.pos; i < current_siz - 1; ++i) {
                storage[i] = storage[i + 1];
            }
            storage[current_siz-1].~T();
            --current_siz;
            if (pos.pos == current_siz - 1) return end();
            else return pos;
        }

        /**
         * removes the element with index ind.
         * return an iterator pointing to the following element.
         * throw index_out_of_bound if ind >= size
         */
        iterator erase(const size_t &ind) {
            if (ind >= current_siz) throw index_out_of_bound();
            for (int i = ind; i < current_siz - 1; ++i) {
                storage[i] = storage[i + 1];
            }
            storage[current_siz-1].~T();
            --current_siz;
            if (ind == current_siz) return end();
            else return iterator(this, ind);
        }

        /**
         * adds an element to the end.
         */
        void push_back(const T &value) {
            if (current_siz == siz) resize();
            new (storage+current_siz)T(value);
            current_siz++;
        }

        /**
         * remove the last element from the end.
         * throw container_is_empty if size() == 0
         */
        void pop_back() {
            if (size() == 0) throw container_is_empty();
            storage[current_siz-1].~T();
            --current_siz;
        }
        T pop(){
            if (size() == 0) throw container_is_empty();
            T tmp=storage[current_siz - 1];
            storage[current_siz-1].~T();
            --current_siz;
            return tmp;
        }
    };



#endif
