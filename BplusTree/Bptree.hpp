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
        class block;

    private:
        node * root;
        int CurrentLen;

    public:
        typedef pair<const Key, T> value_type;
        bool Fewer(Key a, Key b, Compare C = Compare()){
            return C(a, b);
        }
        bool Equal(Key a, Key b){
            return !(Fewer(a,b) || Fewer(b,a));
        }

        class node{
            node * Children, father;
            value_type * data;
            int CurrentChildren;
            node * prev, next;
            int type;
            void AddChildren(node * p){
                if (CurrentChildren < NumOfChildren){
                    Children[CurrentChildren] = p;
                    CurrentChildren++;
                    p->father = this;
                }
                else if (CurrentChildren == NumOfChildren) {
                    //TODO Blance
                }
            }
            node *
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

        bool empty() const {}

        size_t size() const {}

        void clear() {}

        bool insert(const value_type &value) {

        }


        size_t count(const Key &key) const { return CurrentLen;}


    private:
        node * Search(Key key){
            node * p = root;
            while(){

            }
        }
    };

}

#endif
