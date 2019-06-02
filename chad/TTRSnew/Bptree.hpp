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
            /*
             *  Children array could be changed to deque ? which is much faster
             */
            node *Children[IndexSize + 1], *father;

            std::deque<value_type> data;
            int NumChild, type;
            node *prev, *next;

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

            /*
             *  Erase key of node p.
             *  If it's p->data[0] in the indexpage like p->father
             *  then add p->data[1] to indexpage in place of the formal p->data[0]
             *  TODO: improve to binary search
             */
            void BinErase(Key key) {
                int pos = BinSearch(key);
                data.erase(data.begin() + pos);
                if (pos == 0 && father != nullptr) {
                    pos = father->BinSearch(key);
                    if(pos != -1) father->data[pos] = data[0];
                }
                NumChild--;
            }
            /*
             *  return which pos does key holds on data
             *  if it's index page, it could be used as p->BinSearch(p->data.front());
             *  if p is the first child, return -1;
             *  or key does not exist
             */
            int BinSearch(Key key) {
                if (type) {
                    for (int i = 0; i < NumChild; i++) {
                        if (Equal(key, data[i].first)) return i;
                    }
                } else {
                    for (int i = 0; i < NumChild - 1; i++) {
                        if (Equal(key, data[i].first)) return i;
                    }
                    return -1;
                }
            }

            void UpdateFather(int pos){
                father->data[pos] = data.front();
                return;
            }

            void DeleteChild(int pos){
                data.erase(data.begin() + pos);
                for (int i = pos + 1; i < NumChild - 1; i++) {
                    Children[i] = Children[i + 1];
                }
                NumChild--;
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

            bool Equal(Key a, Key b) {
                return !(Fewer(a, b) || Fewer(b, a));
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
            //if (root != nullptr) PrintNode(root);
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
                    else SplitIndex(p->father);
                    return true;
                }

            }
        }

        /*
         *  Also 3 cases;
         *  Consider Fillfactor = LeafSize/2 or IndexSize/2
         */
        node *erase(const Key key) {
            if (CurrentLen == 0) throw(container_is_empty());
            node *p = Search(key);
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
        /*
         *  Used to Debug:
         *  print all the inserted element from left to right
         */
        void Print(){
            node * p = root;
            while(p->type != 1) {
                p = p->Children[0];
            }
            while(p != nullptr) {
                PrintNode(p);
                p = p->next;
            }
            std::cout << std::endl;
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
        void SplitLeaf(node * p) {
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
        void SplitIndex(node *p) {
            /*
             *  Case 1: p is not root
             *     do it recursively;
             */
            while (p != root) {
                if (p->NumChild > IndexSize) {
                    node *brother = GetBrother(p);
                    p->NumChild = IndexSize / 2 + 1;
                    p->data.pop_back();
                    p->father->data.push_back(p->data[IndexSize / 2]); //push_back????
                    p->father->Children[p->father->NumChild] = brother; //p->father->Numchild ????
                    brother->father = p->father;
                    p->father->NumChild++;
                    p = p->father;
                } else break;
            }
            if (p == root && p->NumChild > IndexSize) {
                SplitIndexRoot(p);
            }
        }

        /*
         *  Split index page while it's root
         */
        inline void SplitIndexRoot(node *p) {
            /*
             *  Case 2: p is root of the tree
             */
            node *fa = new node(0);
            fa->NumChild = 2;
            node *brother = GetBrother(p);
            p->NumChild = IndexSize / 2 + 1;

            fa->data.push_back(p->data[IndexSize / 2]);
            for (int i = IndexSize / 2; i < IndexSize; i++) p->data.erase(p->data.begin() + i);
            fa->Children[0] = p;
            p->father = fa;
            fa->Children[1] = brother;
            brother->father = fa;
            root = fa;
        }

        /*
         * Serve for SplitIndexRoot.
         * Do not use REFERENCE!!
         */
        node *GetBrother(node *p) {
            node *brother = new node(0);
            for (int i = IndexSize / 2 + 1; i <= IndexSize; i++) {
                brother->Children[i - IndexSize / 2 - 1] = p->Children[i];
                p->Children[i]->father = brother;
            }
            brother->NumChild = IndexSize - IndexSize / 2;
            for (int i = IndexSize / 2 + 1; i < IndexSize; i++) {
                brother->data.push_back(p->data[i]);
            }
            for (int i = IndexSize / 2 + 1; i < IndexSize; i++) {
                p->data.pop_back();
            }
            return brother;
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
        void RotateLeafPage(node *p) {

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
        bool LendMergeLeaf(node *p) {
            int sit = 0;
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
            }
        }
        /*
         *  Merge leaf page p and p->next
         *  and update p->father->data as well as p->father->Children
         */
        void MergeLeafPage(node *p){
            node * nxt = p->next;
            node * fa = p->father;
            for (int i = p->NumChild; i < p->NumChild + nxt->NumChild; i++){
                p->data.push_back(nxt->data[i - p->NumChild]);
            }
            p->NumChild += nxt->NumChild;
            int pos = fa->BinSearch(nxt->data.front().first);
            fa->DeleteChild(pos);
            p->next = nullptr;
            if (nxt->next != nullptr) {
                p->next = nxt->next;
                nxt->next->prev = p;
            }
            delete nxt;
        }
        /*
         *  Most of the time, just lend from brother
         *  else merge node p with brother
         *  Case 0: Merge
         *  Case 1: Lend from left brother
         *  Case 2: Lend from right brother
         */
        void LendMergeIndex(node * p) {
            if (p == root) {
                /*
                 *  Special case: leaf page is right under root
                 */
                if (p->NumChild != 2) return;
                else {
                    if (p->Children[0]->NumChild == 2 && p->Children[1]->NumChild == 2){
                        EraseRoot();
                    }
                }
                return;
            }
            node * fa = p->father;
            int sit = 0;
            int pos = fa->BinSearch(p->data.front().first);
            if (pos >= 0) {
                if (fa->Children[pos]->NumChild - 1 > IndexSize / 2) sit = 1;
            }
            if (!sit) {
                if (pos + 1 < fa->NumChild - 1) {
                    if (fa->Children[pos + 2]->NumChild - 1 > IndexSize / 2) sit = 2;
                }
            }
            switch (sit){
                case 0: {
                    if (fa == root && fa->NumChild == 2) {
                        EraseRoot(); return;
                    }
                    if (fa->Children[pos + 1] != nullptr) MergeIndexPage(p, pos);
                    else MergeIndexPage(fa->Children[pos - 1], pos - 1);
                    if (fa->NumChild < IndexSize / 2) {
                        LendMergeIndex(fa);
                    }
                    break;
                }
                case 1: {
                    /*
                     *  This case is not complete yet...
                     */
                    node * brother = fa->Children[pos - 1];
                    node * grandson = p->Children[0]->prev;
                    grandson->father = p;
                    for (int i = 1; i < p->NumChild; i++) {

                    }
                    p->Children[0] = grandson;
                    p->data.push_front(grandson->data.front());
                    brother->data.pop_back();
                    brother->NumChild--;
                    p->NumChild--;
                    int pos_p = fa->BinSearch(grandson->Children[0]->data.front().first);
                    break;
                }
                case 2: {
                    /*
                     *  Lend from right brother
                     */
                    node * brother = fa->Children[pos + 2];
                    node * grandson = brother->Children[0];
                    grandson->father = p;
                    p->Children[p->NumChild] = grandson;
                    p->NumChild++;
                    p->data.push_back(grandson->data.front());
                    brother->data.pop_front();
                    for (int i = 0; i < brother->NumChild - 1; i++) {
                        brother->Children[i] = brother->Children[i + 1];
                    }
                    brother->NumChild--;
                    int pos_p = fa->BinSearch(grandson->data.front().first);
                    fa->data[pos_p] = grandson->next->data.front();
                    break;
                }
            }
        }

        /*
         *  p & p->next are both Indexsize / 2
         *  Merge them and update the value f their father.
         */
        void MergeIndexPage(node * p, int pos){
            node * fa = p->father;
            node * brother = fa->Children[pos + 2];
            for (int i = p->NumChild; i < p->NumChild + brother->NumChild; i++) {
                p->Children[i] = brother->Children[i - p->NumChild];
                brother->Children[i - p->NumChild]->father = p;
            }
            p->data.push_back(fa->data[pos]);
            for (int i = 0; i < brother->NumChild; i++) {
                p->data.push_back(brother->data[i]);
            }
            fa->DeleteChild(pos);
            p->NumChild += brother->NumChild;
            fa->NumChild--;

        }
        /*
         *  Root only has 2 children, and they are both less then IndexSize / 2
         *  So we can merge them to a new root
         *  And this is the only situation that a bptree's height becomes lower
         */
        void EraseRoot(){
            if (root->Children[0]->type == 1) {
                node * p = root->Children[0];
                node * bro = root->Children[1];
                for (int i = 0; i < bro->NumChild; i++) {
                    p->data.push_back(bro->data[i]);
                }
                p->NumChild += bro->NumChild;
                delete root;
                delete bro;
                root = p;
                return;
            }
            node * p = root->Children[0];
            node * brother = root ->Children[1];
            p->father = nullptr;
            p->data.push_back(root->data.front());
            for (int i = 0; i < brother->NumChild - 1; i++) {
                p->data.push_back(brother->data[i]);
            }
            for (int i = p->NumChild; i < p->NumChild + brother->NumChild; i++) {
                p->Children[i] = brother->Children[i - p->NumChild];
                p->Children[i]->father = p;
            }
            p->NumChild += brother->NumChild;
            delete brother;
            delete root;
            root = p;
        }

        void PrintNode(node *p) {
            //std::cout << CurrentLen << ": ";
            for (int i = 0; i < p->NumChild; i++) {
                std::cout << p->data[i].first << " ";
            }
            //std::cout << "\n";
        }
    };

}

#endif
