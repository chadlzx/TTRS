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
            int PageSize = 5
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
            friend class Bptree;
            node * Children[NumOfChildren], * father;
            value_type * data; //type == 0 只需存key
            int SizeC;
            node * prev, * next;
            int type; // type (0: indexpage) (1: leafpage)

            void BinarySearchInsert(value_type value){
                if (type == 1){
                    //TODO Improve Efficiency
                    int pos, flag = 0;
                    for (int i = 0; i < SizeC; i++){
                        if ( Fewer(value.first, data[i].first)){
                            pos = i; flag = 1; break;
                        }
                    }
                    if (!flag) pos = SizeC;
                    for (int i = SizeC; i > pos; i--){
                        new(data+i)value_type(data[i-1]);
                    }
                    new(data+pos)value_type(value);
                    SizeC++;
                }
            }

            node(int t, node * f = nullptr, node * p = nullptr, node * n = nullptr):
            type(t), father(f), prev(p), next(n) {
                SizeC = 0;
                if (type == 0){
                    data = (value_type * )operator new(sizeof(value_type) * (NumOfChildren-1) );
                }
                else if (type){
                    data = (value_type *)operator new(sizeof(value_type) * PageSize);
                }
            }
            bool Fewer(Key a, Key b, Compare C = Compare()){
                return C(a, b);
            }
        };


        Bptree() {
            root = nullptr;
            CurrentLen = 0;
        }

        //Bptree(const Bptree &other) {}


        Bptree &operator=(const Bptree &other) {}


        ~Bptree() {}


        T &at(const Key &key) {
            if (CurrentLen == 0) throw(container_is_empty());
            node * p = Search(key);
            for (int i = 0; i < p->SizeC; i++){
                if (p->data[i].first == key) {
                    return p->data[i].second;
                }
            }
            throw(runtime_error());
        }

        T &operator[](const Key &key) {}

        bool empty() const { return (CurrentLen == 0);}

        size_t size() const {return CurrentLen; }

        void clear() {
            ClearNode(root);
            CurrentLen = 0;
        }

        void ClearNode(node * p){
            for (int i = 0; i < p->SizeC; i++){
                ClearNode(p->Children[i]);
            }
            delete p->data; delete p;
        }

        bool insert(const value_type &value) {
            CurrentLen++;

            if (root == nullptr){
                node * Leaf = new node(1);
                new(Leaf->data)value_type(value); Leaf->SizeC++;
                root = Leaf;
                return true;
            } //Initialize

            node * p = Search(value.first);
            if (p->SizeC + 1 < PageSize){
                new (p->data + p->SizeC)value_type(value);
                p->SizeC++;
            }
            else{
                if (p == root){
                    p->BinarySearchInsert(value);
                    node * NewLeaf = new node (1);
                    for (int i = PageSize/2; i <= PageSize-1; i++){
                        new(NewLeaf->data+i-PageSize/2)value_type(p->data[i]);
                    }
                    p->SizeC = PageSize/2;
                    NewLeaf->SizeC = PageSize-PageSize/2;
                    node * fa = new node (0);
                    fa->SizeC = 2;
                    new(fa->data)value_type(NewLeaf->data[0]);
                    fa->Children[0] = p; fa->Children[1] = NewLeaf;
                    p->father = fa; NewLeaf->father = fa;
                    p->next = NewLeaf; NewLeaf->prev = p; //TODO p->next->prev = NewLeaf
                    root = fa;
                    return true;
                }
                if (p->father->SizeC < NumOfChildren){
                    p->BinarySearchInsert(value);
                    split(p);
                    return true;
                }
                else{
                    
                }

            }
        }

        node * erase(const Key key){
            node * p = Search(key);
            if (p->SizeC-1 >= PageSize/2 ){
                p->SizeC--;
                return p;
            }
            else{
                if (p->father->SizeC >= NumOfChildren /2 ){
                    //TODO Index page not delow fill factor
                }
                else {
                    //TODO else
                }

            }
        }


        size_t count(const Key &key) const { return CurrentLen;}


    private:

        node * Search(Key key){
            if (root == nullptr) return nullptr;
            node * p = root;
            int flag;
            while(p->type != 1){
                flag = 0;
                for (int i = 0; i < p->SizeC-1; i++){
                    if (Fewer(key, p->data[i].first)){
                        p = p->Children[i];
                        flag = 1; break;
                    }
                }
                if(!flag) p = p->Children[p->SizeC-1];
            }
            return p;
        }
        
        void split(node * &p){
            node * NewLeaf = new node (1);
            for (int i = PageSize/2; i <= PageSize-1; i++){
                new(NewLeaf->data+i-PageSize/2)value_type(p->data[i]);
            }
            p->SizeC = PageSize/2;
            NewLeaf->SizeC = PageSize-PageSize/2;
            NewLeaf->father = p->father;
            p->father->Children[p->father->SizeC] = NewLeaf;
            p->father->SizeC++;
            node *nextp = p->next;
            p->next = NewLeaf; NewLeaf->prev = p;
            if (nextp != nullptr) {
                nextp->prev = NewLeaf; NewLeaf->next = nextp;
            }
        }

    };

}

#endif
