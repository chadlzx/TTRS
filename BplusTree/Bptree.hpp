#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include "./utility.hpp"
#include "./exceptions.hpp"

#ifndef BPTREE_HPP
#define BPTREE_HPP

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>,
            int NumOfChildren = 5,
            int DataSize = 5
    >
    class Bptree {
    public:
        class node;

    private:
        node * root;
        int CurrentLen;

    public:
        typedef pair<const Key, T> value_type;
        bool Fewer(Key a, Key b, Compare C = Compare()){
            return C(a, b);
        }

        class node{
            node * Children;
            value_type data;
            int CurrentChildren;

            node(){
                Children = (node *)operator new(sizeof(node) * NumOfChildren);
                CurrentChildren = 0;
            }
            ~node(){
                operator delete(Children);
            }
        };
        class iterator {
        private:

        public:
            iterator() {
            }

            iterator(const iterator &other) {
            }

            iterator operator++(int) {}

            iterator &operator++() {}

            iterator operator--(int) {}

            iterator &operator--() {}

            value_type &operator*() const {}

            bool operator==(const iterator &rhs) const {}

            bool operator!=(const iterator &rhs) const {}

            value_type *operator->() const noexcept {}
        };


        Bptree() {
            root = nullptr;
            CurrentLen = 0;
        }

        Bptree(const Bptree &other) {}


        Bptree &operator=(const Bptree &other) {}


        ~Bptree() {}


        T &at(const Key &key) {}

        const T &at(const Key &key) const {}


        T &operator[](const Key &key) {}


        const T &operator[](const Key &key) const {}


        iterator begin() {}

        iterator end() {}

        bool empty() const {}

        size_t size() const {}

        void clear() {}

        pair<iterator, bool> insert(const value_type &value) {

        }

        void erase(iterator pos) {}

        size_t count(const Key &key) const { return CurrentLen;}

        iterator find(const Key &key) {}

    private:
        node * Find(Key key){
            node * p = root;
            if ()
        }
    };

}

#endif
