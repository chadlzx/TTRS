#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <deque>
#include <cassert>
#include <fstream>
#include "./utility.hpp"
#include "./exceptions.hpp"

#ifndef BPTREE_HPP
#define BPTREE_HPP

#define CAST reinterpret_cast<char *>
constexpr off_t invalid_off = 0xdeadbeef;
namespace sjtu {
    /*
     *  Attention : In this file I use std::pair & std::map
     *     replace them with sjtu:pair when finishing development.
     */
    template<
            class Key,
            class T,
            class Compare = std::less<Key>,
            int IndexSize = 5,
            int PageSize = 5
    >
    class Bptree {
    public:
        class node;

    private:
        off_t headLeaf, tailLeaf;
        node * root;
        int CurrentLen;
        char * filename;
        std::fstream file;
        /*
         *  initial state: read information from file if it exists
         */
        inline void init() {
            tailLeaf = invalid_off;
            headLeaf = invalid_off;
            root = nullptr;
        }


        void read_info() {
            file.seekg(0);
            if(root == nullptr) {
                root = new node();
            }
            file.read(CAST(root), sizeof(node));
            file.seekg(sizeof(node));
            file.read(CAST(&CurrentLen), sizeof(int));
            file.seekg(sizeof(node) + sizeof(int));
            file.read(CAST(&headLeaf), sizeof(off_t));
            file.seekg(sizeof(node) + sizeof(int) + sizeof(off_t));
            file.read(CAST(&tailLeaf), sizeof(off_t));
        }

        void save_node(node * p) {
            file.seekp(p->pos);
            file.write(CAST(p), sizeof(node));
        }

        void save_main() {
            file.seekp(0);
            if(root != nullptr) file.write(CAST(root), sizeof(node));
            file.seekp(sizeof(node));
            file.write(CAST(&CurrentLen), sizeof(int));
            file.seekp(sizeof(node) + sizeof(int));
            file.write(CAST(&headLeaf), sizeof(off_t));
            file.seekp(sizeof(node) + sizeof(int) + sizeof(off_t));
            file.write(CAST(&tailLeaf), sizeof(off_t));
        }

        node * NewNode(int Type = 1) {
            node  * newleaf = new node(Type);
            file.seekp(0, std::ios::end);
            auto a1 = file.tellp();
            file.write(CAST(newleaf), sizeof(node));
            file.seekp(0);
            auto a2 = file.tellp();
            newleaf->pos = off_t(a1 - a2);
            return newleaf;
        }

        node * Getnode(off_t p) {
            file.seekg(p);
            node * target = new node();
            file.read(CAST(target), sizeof(node));
            return target;
        }

        void SplitLeafRoot(node * p) {
            node * NewLeaf = NewNode(1);
            for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                NewLeaf->data[i-PageSize/2] = p->data[i];
            }
            /*for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                p->BinErase(i);
            }*/
            p->NumChild = PageSize / 2;
            NewLeaf->NumChild = PageSize - PageSize / 2;
            node * fa = NewNode(0);
            fa->NumChild = 2;
            fa->data[0] = NewLeaf->data[0];
            fa->Children[0] = p->pos;
            fa->Children[1] = NewLeaf->pos;
            p->father = fa->pos;
            NewLeaf->father = fa->pos;
            p->next = NewLeaf->pos;
            NewLeaf->prev = p->pos; //TODO p->next->prev = NewLeaf
            root = fa;
        }

        void SplitIndexRoot(node * p) {

        }

        void SplitIndex (node * p) {

        }

        void SplitLeaf (node * p) {

        }

    public:
        typedef std::pair<Key, T> value_type;
        
        class node{
        public:
            int NumChild, type;
            off_t father, pos;
            int sizeofnode;
            off_t prev, next;
            value_type data[PageSize + 1];
            off_t Children[IndexSize + 1];
            node(int t = 0) : type(t){
                NumChild = 0;
                father = invalid_off;
                pos = invalid_off;
                prev = invalid_off;
                next = invalid_off;
            }

            void BinInsert(value_type value) {
                if (type == 1) {
                    data[NumChild] = value;
                    for (int i = NumChild; i >= 1; i--) {
                        if (data[i] < data[i - 1]) {
                            std::swap(data[i], data[i - 1]);
                        } else break;
                    }
                    NumChild++;
                }
            }

            void BinErase(int pos) {
                for (int i = pos; i < NumChild; i++) {
                    data[i] = data[i+1];
                }
            }
        };
        /*
         * TODO: Improve to 2  classes
         */
        class indexnode : node {

        };

        class leafnode : node {

        };
        
        bool Fewer(Key a, Key b, Compare C = Compare()) {
            return C(a, b);
        }

        bool Equal(Key a, Key b) {
            return !(Fewer(a, b) || Fewer(b, a));
        }
        
        Bptree(const char * fname) {
            init();
            filename = new char[strlen(fname) + 1];
            strcpy(filename, fname);

            file.open(fname, std::fstream::out|std::fstream::in|std::fstream::binary);
            if(!file) {
                std::cout << "file not exist...\n";
                CurrentLen = 0;
                file.open(fname, std::fstream::out|std::fstream::binary);
                std::cout << "new file <" << fname << "> has been added to dir\n";
                file.close();
                file.open(fname);
                save_main();
            }
            else {
                std::cout << "file exist...\n";
                std::cout << "reading file...\n";
                read_info();
                std::cout << "Current length is " << CurrentLen << "\n";
            }
        }

        ~Bptree() {
            save_main();
            delete root;
            delete filename;
            file.close();
        }

        int size(){
            return CurrentLen;
        }

        T at(const Key &key) {
            if (CurrentLen == 0) throw (container_is_empty());
            node * p = Search(key);
            for (int i = 0; i < p->NumChild; i++) {
                if (p->data[i].first == key) {
                    return p->data[i].second;
                }
            }
            throw (runtime_error());
        }

        node * Search(Key key) {
            if (CurrentLen == 0) throw(container_is_empty());
            node * p = root;
            int flag;
            while (p->type != 1) {
                flag = 0;
                for (int i = 0; i < p->NumChild - 1; i++) {
                    if (Fewer(key, p->data[i].first)) {
                        off_t nextp = p->Children[i];
                        p = Getnode(nextp);
                        flag = 1;
                        break;
                    }
                }
                if (!flag) {
                    off_t nextp = p->Children[p->NumChild - 1];
                    p = Getnode(nextp);
                }
            }
            return p;
        }



        void insert(value_type value) {
            CurrentLen++;
            /*
             *  Insert first element
             */
            if (CurrentLen == 1) {
                node * newleaf = NewNode();
                newleaf->NumChild++;
                newleaf->data[0] = value;
                save_node(newleaf);
                root = newleaf;
                return;
            }

            node * p = Search(value.first);
            /*
             *  Case 1: the leaf page isn't full yet
             *          insert directly
             */
            if (p->NumChild + 1 < PageSize) {
                p->BinInsert(value);
                save_node(p);
                return;
            } else {
                /*
                 *  Case 2: the leaf page is full
                 *     But the index page above is not full
                 *     SubCase 1: leaf page is root
                 *     SubCase 2: else
                 */
                if (p->NumChild + 1 < PageSize) {
                    p->BinInsert(value);
                    return;
                } else {
                    /*
                     *  Case 2: the leaf page is full
                     *     But the index page above is not full
                     *     SubCase 1: leaf page is root
                     *     SubCase 2: else
                     */

                    if (p == root) {
                        p->BinInsert(value);
                        SplitLeafRoot(p);
                        return;
                    } else {
                        node * fa = Getnode(p->father);
                        if (fa->NumChild < IndexSize) {
                        p->BinInsert(value);
                        SplitLeaf(p);
                        /*
                         *  TODO : if p->prev not full, then rotate p
                         */
                        return;
                    } else {
                        /*
                         *  Case 3: both the leaf page & the index page are full
                         *      we should balance the tree recursively
                         */
                        p->BinInsert(value);
                        SplitLeaf(p);
                        if (p->father == root->pos) SplitIndexRoot(fa);
                        else SplitIndex(fa);
                        return;
                    }
                }

                }
            }
        }

        void erase (Key key) {

        }


        /*void traverse(){};
        void count(Key key) {};*/
    };
}

#endif