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
            int type; // type (0: node) (1: datablcok)
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
            node(int t, node * f = nullptr, node * p = nullptr, node * n = nullptr):
            type(t), father(f), prev(p), next(n) {
                if (type == 0){
                    data = (value_type * )operator new(sizeof(value_type) * (NumOfChildren-1) );
                    Children = new node [NumOfChildren];
                }
                else if (type){
                    data = (value_type *)operator new(sizeof(value_type) * DataSize);
                    Children = nullptr;
                }
            }
        };


        Bptree() {
            root = nullptr;
            CurrentLen = 0;
        }

        //Bptree(const Bptree &other) {}


        Bptree &operator=(const Bptree &other) {}


        ~Bptree() {}


        T &at(const Key &key) {}

        T &operator[](const Key &key) {}

        bool empty() const { return (CurrentLen == 0);}

        size_t size() const {return CurrentLen; }

        void clear() {
            ClearNode(root);
            CurrentLen = 0;
        }

        void ClearNode(node * p){
            for (int i = 0; i < p->CurrentChildren; i++){
                ClearNode(p->Children[i]);
            }
            delete p->data; delete p;
        }

        bool insert(const value_type &value) {
            node * p = Search(value.first);
            if (p->CurrentChildren < DataSize){
                p->data[p->CurrentChildren] = value;
                p->CurrentChildren++;
            }
            else{
                //TODO Balance
            }
        }

        node * erase(const Key key){
            node * p = Search(key);
            if (p->CurrentChildren-1 >= DataSize/2 ){
                //p->data[p->CurrentChildren-1] 要析构吗
                p->CurrentChildren--;
            }
            else{
                //TODO BALANCE
            }
        }


        size_t count(const Key &key) const { return CurrentLen;}


    private:
        node * Search(Key key){
            node * p = root;
            while(p->type != 1){
                for (int i = 0; i < p->CurrentChildren-1; i++){
                    if (Fewer(key, p->data[i])){
                        p = p->Children[i];
                    }
                }
            }
            return p;
        }

    };

}

#endif
