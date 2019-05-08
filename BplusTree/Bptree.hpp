#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <deque>
#include <cassert>
#include "./utility.hpp"
#include "./exceptions.hpp"

#ifndef BPTREE_HPP
#define BPTREE_HPP

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

        class block;

    private:
        node *root;
        int CurrentLen;

    public:
        typedef std::pair<Key, T> value_type;

        bool Fewer(Key a, Key b, Compare C = Compare()) {
            return C(a, b);
        }

        bool Equal(Key a, Key b) {
            return !(Fewer(a, b) || Fewer(b, a));
        }

        class node {
            /*
             *  type = 0 {
             *      it's an index page, it stores the address of its (NumChild) children
             *      as well as (NumChild - 1) index to redirect to its children
             *      prev&next are not needed in this type
             *  }
             *  type = 1 {
             *      it's a leaf page, it stores (PageSize) values in data, and it has prev&next
             *  }
             */
            friend class Bptree;

            node *Children[IndexSize + 1], *father;
            std::deque<value_type> data;
            int NumChild;
            node *prev, *next;
            int type;

            /*
             *  Insert value into a leaf page not considering it's full or not
             *  TODO: improve to binary search
             */
            void BinInsert(value_type value) {
                if (type == 1) {
                    data.push_back(value);
                    for (int i = NumChild; i >= 1; i--) {
                        if (data[i] < data[i - 1]) {
                            std::swap(data[i], data[i - 1]);
                        } else break;
                    }
                    NumChild++;
                }
            }

            node(int t, node *f = nullptr, node *p = nullptr, node *n = nullptr) :
                    type(t),
                    father(f),
                    prev(p),
                    next(n) {
                NumChild = 0;
            }

            bool Fewer(Key a, Key b, Compare C = Compare()) {
                return C(a, b);
            }
        };


        Bptree() {
            root = nullptr;
            CurrentLen = 0;
        }


        Bptree &operator=(const Bptree &other) {}


        ~Bptree() {}


        T &at(const Key &key) {
            if (CurrentLen == 0) throw (container_is_empty());
            node *p = Search(key);
            for (int i = 0; i < p->NumChild; i++) {
                if (p->data[i].first == key) {
                    return p->data[i].second;
                }
            }
            throw (runtime_error());
        }

        T &operator[](const Key &key) {}

        bool empty() const { return (CurrentLen == 0); }

        size_t size() const { return CurrentLen; }

        void clear() {
            ClearNode(root);
            CurrentLen = 0;
        }

        void ClearNode(node *p) {
            for (int i = 0; i < p->NumChild; i++) {
                ClearNode(p->Children[i]);
            }
            delete p;
        }

        /*
         *  the most shitty function
         *  there're 3 cases in total
         */
        bool insert(const value_type &value) {
            if(root != nullptr) PrintNode(root);
            CurrentLen++;
            /*
             *  Insert first element
             */
            if (root == nullptr) {
                node *Leaf = new node(1);
                Leaf->data.push_back(value);
                Leaf->NumChild++;
                root = Leaf;
                return true;
            }
            node *p = Search(value.first);
            /*
             *  Case 1: the leaf page isn't full yet
             *          insert directly
             */
            if (p->NumChild + 1 < PageSize) {
                p->BinInsert(value);
                return true;
            } else {
                /*
                 *  Case 2: the leaf page is full
                 *     But the index page above is not full
                 *     SubCase 1: leaf page is root
                 *     SubCase 2: else
                 */
                if (p == root) {
                    p->BinInsert(value);
                    SplitLeafRoot(value, p);
                    return true;
                } else if (p->father->NumChild < IndexSize) {
                    p->BinInsert(value);
                    SplitLeaf(p);
                    /*
                     *  TODO : if p->prev not full, then rotate p
                     */
                    return true;
                } else {
                    /*
                     *  Case 3: both the leaf page & the index page are full
                     *      we should balance the tree recursively
                     */
                    p->BinInsert(value);
                    SplitLeaf(p);
                    if (p->father == root) SplitIndexRoot(p->father);
                }

            }
        }

        /*
         *  Also 3 cases;
         *  Case 1: LeafPageSize > PageSize / 2; Just erase directly
         *  Case 2: else
         */
        node *erase(const Key key) {
            node *p = Search(key);
            if (p->NumChild - 1 >= PageSize / 2) {
                p->NumChild--;
                return p;
            } else {
                if (p->father->NumChild >= IndexSize / 2) {
                } else {

                }

            }
        }


        size_t count(const Key &key) const { return CurrentLen; }


    private:
        /*
         *  Find the block that key might be at
         *  If there is no block, return root
         */
        node *Search(Key key) {
            if (root == nullptr) return nullptr;
            node *p = root;
            int flag;
            while (p->type != 1) {
                flag = 0;
                for (int i = 0; i < p->NumChild - 1; i++) {
                    if (Fewer(key, p->data[i].first)) {
                        p = p->Children[i];
                        flag = 1;
                        break;
                    }
                }
                if (!flag) p = p->Children[p->NumChild - 1];
            }
            return p;
        }

        /*
         *  SplitLeaf a max-size node p into 2 nodes
         *  and link them with their father (maintain index !)
         */
        void SplitLeaf(node * &p) {
            /*
             *  TODO Debug
             */
            node *NewLeaf = new node(1);
            node *fa = p->father;
            for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                NewLeaf->data.push_back(p->data[i]);
            }

            p->NumChild = PageSize / 2;
            NewLeaf->NumChild = PageSize - PageSize / 2;
            NewLeaf->father = fa;
            fa->Children[fa->NumChild] = NewLeaf;
            fa->NumChild++;
            fa->data.push_back(NewLeaf->data[0]);
            //fa->data[p->father->NumChild - 3] = p->data[0];
            node *nextp = p->next;
            p->next = NewLeaf;
            NewLeaf->prev = p;
            if (nextp != nullptr) {
                nextp->prev = NewLeaf;
                NewLeaf->next = nextp;
            }
            for (int i = PageSize / 2; i <= PageSize - 1; i++) p->data.erase(p->data.begin() + i);
        }

        /*
         *  Split a index-page into 2 index-pages, then create a
         *  link to their father index page.
         *  For example, index page holds (3, 6, 9, 12, 15) which connect 6 leaf pages
         *  it turns to
         *           9
         *        /     \
         *    (3, 6)  (12, 15)
         */
        void SplitIndex(node *&p) {
            /*
             *  Case 1: p is not root
             *     do it recursively;
             */
        }

        /*
         *  Split index page while it's root
         */
        void SplitIndexRoot(node *&p) {
            /*
             *  Case 2: p is root of the tree
             */
            node *fa = new node(0);
            fa->NumChild = 2;
            node *brother = new node(0);
            for (int i = IndexSize / 2 + 1; i <= IndexSize; i++) {
                brother->Children[i - IndexSize / 2 - 1] = p->Children[i];
                p->Children[i]->father = brother;
                /*
                 *  why suddenly changed
                 *  it's so freaking weird
                 */
            }
            brother->NumChild = IndexSize - IndexSize / 2;
            p->NumChild = IndexSize / 2 + 1;
            for (int i = IndexSize / 2 + 1; i < IndexSize; i++) {
                brother->data.push_back(p->data[i]);
            }
            fa->data.push_back(p->data[IndexSize / 2]);
            for (int i = IndexSize / 2; i < IndexSize; i++) p->data.erase(p->data.begin() + i);
            fa->Children[0] = p;
            p->father = fa;
            fa->Children[1] = brother;
            brother->father = fa;
            root = fa;
        }

        /*
         *  Initial situation that leafpage is root
         *  Insert and develop new root
         */
        inline void SplitLeafRoot(const value_type &value, node *&p) {
            node *NewLeaf = new node(1);
            for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                NewLeaf->data.push_back(p->data[i]);
            }
            for (int i = PageSize / 2; i <= PageSize - 1; i++) p->data.erase(p->data.begin() + i);
            p->NumChild = PageSize / 2;
            NewLeaf->NumChild = PageSize - PageSize / 2;
            node *fa = new node(0);
            fa->NumChild = 2;
            fa->data.push_back(NewLeaf->data[0]);
            fa->Children[0] = p;
            fa->Children[1] = NewLeaf;
            p->father = fa;
            NewLeaf->father = fa;
            p->next = NewLeaf;
            NewLeaf->prev = p; //TODO p->next->prev = NewLeaf
            root = fa;
        }

        /*
         *  When leaf page p is full, we can use space from it's brother
         *  by rotation otherwise there'll be too much split and leaf page is not always full
         */
        void RotateLeafPage(node *&p) {

        }

        void PrintNode(node *&p){
            std::cout << CurrentLen << ": ";
            for (int i = 0; i < p->NumChild-1; i++){
                std::cout << p->data[i].first << " ";
            }
            std::cout << "\n";
        }
    };

}

#endif
