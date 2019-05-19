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
            if (p == invalid_off) throw(invalid_offset());
            file.seekg(p);
            node * target = new node();
            file.read(CAST(target), sizeof(node));
            return target;
        }

        /*
         *  Serve for SplitIndexRoot.
         *  Do not use REFERENCE!!
         *  TODO save & get data from files
         */
        node * GetBrother(node * p) {
            node *brother = NewNode(0);
            for (int i = IndexSize / 2 + 1; i <= IndexSize; i++) {
                brother->Children[i - IndexSize / 2 - 1] = p->Children[i];
                node * pchild = Getnode(p->Children[i]);
                pchild->father = brother->pos;
                save_node(pchild);
            }
            brother->NumChild = IndexSize - IndexSize / 2;
            for (int i = IndexSize / 2 + 1; i < IndexSize; i++) {
                brother->data[i - (IndexSize/2 + 1)] = p->data[i];
            }
            return brother;
        }

        inline node * Father(node * p) {
            if (p->father == root->pos) {
                return root;
            }
            else {
                return Getnode(p->father);
            }
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

            save_node(NewLeaf);
            save_node(fa);
            save_node(p);
            root = fa;
        }

        void SplitIndexRoot(node * p) {
            /*
             *  Case 2: p is root of the tree
             *  TODO: Save and get data from files
             */

            node * fa = NewNode(0);
            fa->NumChild = 2;
            node * brother = GetBrother(p);
            p->NumChild = IndexSize / 2 + 1;
            fa->data[0] = p->data[IndexSize / 2];
/*            for (int i = IndexSize / 2; i < IndexSize; i++) {
                p->data.erase(p->data.begin() + i);
            }*/
            fa->Children[0] = p->pos;
            p->father = fa->pos;
            fa->Children[1] = brother->pos;
            brother->father = fa->pos;
            root = fa;

            //save_node(fa);
            save_node(brother);
        }

        void SplitIndex (node * p) {
            /*
             *  Case 1: p is not root
             *     do it recursively;
             */
            while (p != root) {
                if (p->NumChild > IndexSize) {
                    node * brother = GetBrother(p);
                    node * fa = Father(p);
                    p->NumChild = IndexSize / 2 + 1;
                    //p->data.pop_back();
                    //fa->data.push_back(p->data[IndexSize / 2]); //push_back????

                    int pos_p;
                    for (int i = 0; i < fa->NumChild; i++) {
                        if (fa->Children[i] == p->pos) {
                            pos_p = i;
                            break;
                        }
                    }
                    for (int i = pos_p + 1; i < fa->NumChild; i++) {
                        fa->Children[i + 1] = fa->Children[i];
                    }
                    fa->Children[pos_p + 1] = brother->pos;
                    for (int i = pos_p; i < fa->NumChild - 1; i++) {
                        fa->data[i + 1] = fa->data[i];
                    }
                    fa->data[pos_p] = p->data[IndexSize / 2]; //brother->data[0];

                    brother->father = p->father;
                    fa->NumChild++;

                    save_node(brother);
                    save_node(fa);
                    save_node(p);
                    p = fa;
                } else break;
            }
            if (p == root && p->NumChild > IndexSize) {
                SplitIndexRoot(p);
            }

        }

        void SplitLeaf (node * p, node * fa) {
            node * NewLeaf = NewNode(1);
/*            node * fa = nullptr;
            if (p->father == root->pos) {
                fa = root;
            }
            else {
                fa = Getnode(p->father);
            }*/
            for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                NewLeaf->data[i- PageSize / 2] = p->data[i];
            }
            p->NumChild = PageSize / 2;
            NewLeaf->NumChild = PageSize - PageSize / 2;
            NewLeaf->father = fa->pos;
            /* TODO set place for new node in father->children and father->data*/
            int pos_p;
            for (int i = 0; i < fa->NumChild; i++) {
                if (fa->Children[i] == p->pos) {
                    pos_p = i;
                    break;
                }
            }
            for (int i = pos_p + 1; i < fa->NumChild; i++) {
                fa->Children[i + 2] = fa->Children[i + 1];
            }
            fa->Children[pos_p + 1] = NewLeaf->pos;

                for (int i = pos_p; i < fa->NumChild - 1; i++) {
                    fa->data[i + 1] = fa->data[i];
                }
                fa->data[pos_p] = NewLeaf->data[0];

            /* TODO */
            fa->NumChild++;
            off_t PosNextp = p->next;
            p->next = NewLeaf->pos;
            NewLeaf->prev = p->pos;
            if (PosNextp != invalid_off) {
                node * nextp = Getnode(PosNextp);
                nextp->prev = NewLeaf->pos;
                NewLeaf->next = nextp->pos;
                save_node(nextp);
            }
            save_node(fa);
            save_node(p);
            save_node(NewLeaf);
            /*for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                p->data.erase(p->data.begin() + i);
            }*/
        }

        /*
         *  To keep LeafPageSize larger then fillfactor
         *  we consider 3 cases
         *  Case 1: prev is large enough, then take one data of prev to p;
         *  Case 2: next is large(if there is one), then do the same
         *  Case 3: both are not enough, merge prev and p;
         *  if lend is successful return true;
         *  else return false,
         *  and we need to check if the tree is still balanced or p->father is root
         */
        bool LendMergeLeaf(node * p) {
            /*int sit = 0;
            if (p->prev != nullptr) {
                if (p->prev->NumChild > PageSize / 2) sit = 1;
            }
            if (sit == 0){
                if (p->next != nullptr) {
                    if (p->next->NumChild > PageSize / 2) sit = 2;
                    else sit = 3;
                }
            }
            switch (sit) {
                case 1: {
                    node *pre = p->prev;
                    p->BinInsert(pre->data.back());
                    pre->data.pop_back();
                    pre->NumChild--;
                    return true;
                }
                case 2: {
                    node * nxt = p->next;
                    p->BinInsert(nxt->data.front());
                    int pos = nxt->father->BinSearch(nxt->data.front().first);
                    nxt->data.pop_front();
                    nxt->UpdateFather(pos);
                    nxt->NumChild--;
                    return true;
                }
                case 3: {
                    if (p->prev != nullptr) MergeLeafPage(p->prev);
                    else MergeLeafPage(p);
                    return false;
                }
            }*/
        }

        void LendMergeIndex(node * p) {

        }

        void MergeLeafPage (node * p) {

        }

        void MergeIndexPage(node * p) {

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

            void PrintLeafNode() {
                if (type != 1) throw(invalid_format());
                for (int i = 0; i < NumChild; i++) {
                    std::cout << "Information for id " << data[i].first << " is " << data[i].second << "\n";
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
                std::cout << "File exist...\n";
                std::cout << "Reading file...\n";
                read_info();
                std::cout << "Initializing, we get " << CurrentLen << " data-pair currently." << "\n";
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
            std::cout << "A new train is created, the train id is: " << value.first << ". ";
            std::cout << "Train information is: " << value.second << "\n";
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
                        node * fa;
                        if (p->father == root->pos) fa = root;
                        else fa = Getnode(p->father);
                        if (fa->NumChild < IndexSize) {
                        p->BinInsert(value);
                        SplitLeaf(p, fa);
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
                        SplitLeaf(p, fa);
                        if (p->father == root->pos) SplitIndexRoot(fa);
                        else SplitIndex(fa);
                        return;
                    }
                }

                }
            }
        }

        node * erase (Key key) {
            if (CurrentLen == 0) throw(container_is_empty());
            node * p = Search(key);
            if (p == nullptr) throw(invalid_iterator());
            CurrentLen--;
            if (p == root) {
                p->BinErase(key);
                return p;
            }
            if (p->NumChild - 1 >= PageSize / 2) {
                /*
                 * Case 1: LeafPageSize > FillFactor;
                 *      Just erase (and update data of Indexpage)
                 */
                p->BinErase(key);
                return p;
            } else {
                if (p->father->NumChild - 1 > IndexSize / 2) {
                    /*
                     *  Case 2: else if IndexPageSize > FillFactor;
                     *      Get data from brother if brother is larger then fillfactor
                     *      else Merge Leaf p and its brother
                     */
                    p->BinErase(key);
                    LendMergeLeaf(p);
                } else {
                    /*
                     * Case 3: IndexPageSize & LeafPageSize <= FillFactor
                     *      Lend and merge recursively
                     */
                    p->BinErase(key);
                    if( !LendMergeLeaf(p) ) {
                        LendMergeIndex(p->father);
                    }
                }

            }
        }


        void tranverse(){
            std::cout << "Tranversing information for all ids...\n";
            node * p = Getnode(headLeaf);
            while (true) {
                p->PrintLeafNode();
                if (p->pos == tailLeaf) break;
                p = Getnode(p->next);
            }
        }
        //void count(Key key) {};
    };


}

#endif