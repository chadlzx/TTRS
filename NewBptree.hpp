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
    int ans=0;
    template<
            class Key,
            class T,
            int IndexSize = 9,
            int PageSize = 9,
            class Compare = std::less<Key>
    >
    class Bptree {
    public:
        class node;

        friend class iterator;

    private:
        off_t headLeaf, tailLeaf;
        node *root;
        int CurrentLen;
        char *filename;
        std::fstream file;

        /*
         *  initial state: read information from file if it exists
         */
        inline void init() {
            tailLeaf = invalid_off;
            headLeaf = invalid_off;
            root = nullptr;
        }
        inline void read_info() {
            file.seekg(0);
            root = new node();
            file.read(CAST(root), sizeof(node));
            file.seekg(sizeof(node));
            file.read(CAST(&CurrentLen), sizeof(int));
            file.seekg(sizeof(node) + sizeof(int));
            file.read(CAST(&headLeaf), sizeof(off_t));
            file.seekg(sizeof(node) + sizeof(int) + sizeof(off_t));
            file.read(CAST(&tailLeaf), sizeof(off_t));
        }

        void save_node(node *p) {
            file.seekp(p->pos);
            file.write(CAST(p), sizeof(node));
        }

        inline void save_main() {//将root的变量记录，会出错
            file.seekp(0);
            if (root == nullptr) {
                root = NewNode(1); //must not be nullptr
            }
            file.write(CAST(root), sizeof(node));
            file.seekp(sizeof(node));
            file.write(CAST(&CurrentLen), sizeof(int));
            file.seekp(sizeof(node) + sizeof(int));
            file.write(CAST(&headLeaf), sizeof(off_t));
            file.seekp(sizeof(node) + sizeof(int) + sizeof(off_t));
            file.write(CAST(&tailLeaf), sizeof(off_t));
        }

        node *NewNode(int Type = 1) {
            node *newleaf = new node(Type);
            file.seekp(0, std::ios::end);
            auto a1 = file.tellp();
            file.write(CAST(newleaf), sizeof(node));
            file.seekp(0);
            auto a2 = file.tellp();
            newleaf->pos = off_t(a1 - a2);
            return newleaf;
        }

        void FuckNode(node*& p) {
            if (p != nullptr && p != root) {
                delete p;
                p=nullptr;
            }
        }

        node *Getnode(off_t p) {
            if (p == invalid_off) throw (invalid_offset());
            file.seekg(p);
            node *target = new node();
            file.read(CAST(target), sizeof(node));
            return target;
        }

        /*
         *  Serve for SplitIndexRoot.
         *  Do not use REFERENCE!!
         *  TODO save & get data from files
         */
        node *GetBrother(node *p) {//没有问题,自动认为p->NumChild为偶数
            node *brother = NewNode(0);
            for (int i = IndexSize / 2 + 1; i <= IndexSize; i++) {
                brother->Children[i - IndexSize / 2 - 1] = p->Children[i];
                node *pchild = Getnode(p->Children[i]);
                pchild->father = brother->pos;
                save_node(pchild);
                delete pchild;
            }
            brother->NumChild = IndexSize - IndexSize / 2;
            for (int i = IndexSize / 2 + 1; i < IndexSize; i++) {
                brother->data[i - (IndexSize / 2 + 1)] = p->data[i];
            }
            return brother;
        }

        inline node *Father(node *p) {
            if (p->father == root->pos) {
                return root;
            } else {
                return Getnode(p->father);
            }
        }

        void EraseRoot() {//检查完毕

            node *child0 = Getnode(root->Children[0]);
            if (child0->type == 1) {
                node *bro = Getnode(root->Children[1]);

                for (int i = 0; i < bro->NumChild; i++) {
                    child0->data[child0->NumChild + i] = bro->data[i];
                }
				child0->father=invalid_off;//?
                child0->NumChild += bro->NumChild;
                delete root;
                root=child0;
                delete bro;
                save_node(root);
                return;
            }
            node *bro = Getnode(root->Children[1]);
            child0->father = invalid_off;

            for (int i = child0->NumChild ; i < child0->NumChild + bro->NumChild; i++) {
                child0->Children[i] = bro->Children[i - child0->NumChild];
                node *tmp = Getnode(bro->Children[i- child0->NumChild]);//cnm
                tmp->father = child0->pos;
                save_node(tmp);
                delete tmp;
            }

            for (int i = child0->NumChild; i < child0->NumChild + bro->NumChild - 1; i++) {
                child0->data[i] = bro->data[i- child0->NumChild];
            }

            child0->data[child0->NumChild-1] = root->data[0];
            child0->NumChild += bro->NumChild;
            save_node(child0);
            delete bro;
            delete root;
            root = child0;

        }

        void SplitLeafRoot(node *p) {//检索完成,必须此时为奇数numchild
        	assert((p->NumChild&1));
            node *NewLeaf = NewNode(1);
            for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                NewLeaf->data[i - PageSize / 2] = p->data[i];
            }

            p->NumChild = PageSize / 2;
            NewLeaf->NumChild = PageSize - PageSize / 2;
            node *fa = NewNode(0);
            fa->NumChild = 2;
            fa->data[0] = NewLeaf->data[0];
            fa->Children[0] = p->pos;
            fa->Children[1] = NewLeaf->pos;
            p->father = fa->pos;
            NewLeaf->father = fa->pos;

            p->next = NewLeaf->pos;
            p->prev= invalid_off;
            NewLeaf->prev = p->pos; //TODO p->next->prev = NewLeaf
            NewLeaf->next = invalid_off;
            headLeaf = p->pos;
            tailLeaf = NewLeaf->pos;
            save_node(NewLeaf);
            save_node(fa);
            save_node(p);
            root = fa;
            FuckNode(NewLeaf);
            FuckNode(p);
        }

        void SplitIndexRoot(node *p) {//必须为偶数 检查完毕
            /*
             *  Case 2: p is root of the tree
             *  TODO: Save and get data from files
             */
			assert((p->NumChild&1)==0);
            node *fa = NewNode(0);
            fa->NumChild = 2;
            node *brother = GetBrother(p);
            p->NumChild = IndexSize / 2 + 1;
            fa->data[0] = p->data[IndexSize / 2];
            fa->Children[0] = p->pos;
            p->father = fa->pos;
            fa->Children[1] = brother->pos;
            brother->father = fa->pos;


            save_node(fa);
            save_node(p);
            save_node(brother);
            root = fa;

            FuckNode(brother);
            FuckNode(p);
        }

        void SplitIndex(node *p) {
            /*
             *  Case 1: p is not root
             *     do it recursively;
             */
            while (p != root) {//检查完毕
                if (p->NumChild > IndexSize) {
                    node *brother = GetBrother(p);
                    node *fa = Father(p);
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
                    for (int i = fa->NumChild; i > pos_p + 1; i--) {
                        fa->Children[i] = fa->Children[i-1];
                    }
                    fa->Children[pos_p + 1] = brother->pos;
/*                    for (int i = pos_p; i < fa->NumChild - 1; i++) {
                        fa->data[i + 1] = fa->data[i];
                    }*/
                    for (int i = fa->NumChild - 1; i > pos_p; i--) {
                        fa->data[i] = fa->data[i-1];
                    }
                    fa->data[pos_p] = p->data[IndexSize / 2]; //brother->data[0];

                    brother->father = fa->pos;
                    fa->NumChild++;

                    save_node(brother);
                    save_node(fa);
                    save_node(p);
                    FuckNode(p);
                    FuckNode(brother);
                    p = fa;
                } else break;
            }
            if(p!=root&&p->NumChild <= IndexSize){
            	delete p;
            	return ;
            }
            if (p == root && p->NumChild > IndexSize) {
                SplitIndexRoot(p);
            }


        }

        void SplitLeaf(node* p, node *fa) {//一检测
            node *NewLeaf = NewNode(1);
            for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                NewLeaf->data[i - PageSize / 2] = p->data[i];
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
/*            for (int i = pos_p + 1; i < fa->NumChild; i++) {
                fa->Children[i + 1] = fa->Children[i ];
            }*/
            for (int i = fa->NumChild; i > pos_p + 1; i--) {
                fa->Children[i] = fa->Children[i-1];
            }
            fa->Children[pos_p + 1] = NewLeaf->pos;

/*            for (int i = pos_p; i < fa->NumChild - 1; i++) {
                fa->data[i + 1] = fa->data[i];
            }*/
            for (int i = fa->NumChild - 1; i > pos_p; i--) {
                fa->data[i] = fa->data[i-1];
            }
            fa->data[pos_p] = NewLeaf->data[0]; //TODO

            /* TODO */
            fa->NumChild++;
            off_t PosNextp = p->next;
            if (p->pos == tailLeaf) {
                tailLeaf = NewLeaf->pos;
            }
            p->next = NewLeaf->pos;
            NewLeaf->prev = p->pos;

            if (PosNextp != invalid_off) {
                node *nextp = Getnode(PosNextp);
                nextp->prev = NewLeaf->pos;
                NewLeaf->next = nextp->pos;
                save_node(nextp);
                FuckNode(nextp);
            }

            save_node(fa);
            save_node(p);
            save_node(NewLeaf);
            FuckNode(NewLeaf);
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
        bool LendMergeLeaf(node*& p, node* fa) {
            int sit = 3;
            node *pre = nullptr, *nxt = nullptr;
            if (p->prev != invalid_off) {
                pre = Getnode(p->prev);
                if (pre->NumChild > PageSize / 2) sit = 1;
            }
            if (sit == 3) {
                if (p->next != invalid_off) {
                    nxt = Getnode(p->next);
                    if (nxt->NumChild > PageSize / 2) sit = 2;

                }
            }
            switch (sit) {
                case 1: {
                    p->BinInsert(pre->data[pre->NumChild - 1]);
                    pre->NumChild--;
                    save_node(pre);
                    save_node(p);
                    FuckNode(pre);
                    return true;
                }
                case 2: {
                    p->BinInsert(nxt->data[0]);
                    int pos = fa->BinSearch(nxt->data[0].first);
                    for (int i = 0; i < nxt->NumChild - 1; i++) {
                        nxt->data[i] = nxt->data[i + 1];
                    }
                    if(pos)fa->data[pos - 1] = p->data[0];
                    fa->data[pos] = nxt->data[0];
                    nxt->NumChild--;
                    save_node(nxt);
                    save_node(p);
                    save_node(fa);
                    FuckNode(pre);
                    FuckNode(nxt);
                    return true;
                }
                case 3: {
                    int pos_p = fa->PosSearch(p->pos);
                    // pos_p != fa->Numchild is better
                    if (p->next != invalid_off && pos_p != fa->NumChild - 1) {
                        MergeLeafPage(p, nxt, fa);
                        FuckNode(pre);
                        FuckNode(nxt);
                    }
                    else {
                        MergeLeafPage(pre, p, fa);
                        FuckNode(nxt);
                        save_node(pre);
                        FuckNode(p);
                        p=pre;
                    }
                    return false;
                }
            }
        }

        /*
         *  Most of the time, just lend from brother
         *  else merge node p with brother
         *  Case 0: Merge
         *  Case 1: Lend from left brother
         *  Case 2: Lend from right brother
         */
        void LendMergeIndex(node*  p) {//p自行處理
            if (p == root) {
                /*
                 *  Special case: leaf page is right under root
                 */
                if (p->NumChild == 1) {
                    root = Getnode(p->Children[0]);
                    FuckNode(p);
                    root->father = invalid_off;
                    save_node(root);
                    return;
                }
                if (p->NumChild != 2) return; //Pagesize / 2 ???
                else {
                    node *child0 = Getnode(p->Children[0]);
                    node *child1 = Getnode(p->Children[1]);
                    if (child0->NumChild == IndexSize / 2 && child1->NumChild == IndexSize / 2) {
                        EraseRoot();
                    }
                    FuckNode(child0);
                    FuckNode(child1);
                }
                return;
            }
            node *fa = Father(p);
            node *brother = nullptr;
            if(fa==root){
                //printf("嘿嘿嘿\n");
            }
            int sit = 0;
            int pos = fa->PosSearch(p->pos);
            if (pos >= 1) {
                brother = Getnode(fa->Children[pos - 1]);
                if (brother->NumChild - 1 > IndexSize / 2) sit = 1;
                else FuckNode(brother);
            }
            if (!sit) {
                if (pos + 1 < fa->NumChild) {
                    brother = Getnode(fa->Children[pos + 1]);
                    if (brother->NumChild - 1 > IndexSize / 2) sit = 2;
                    else FuckNode(brother);
                }
            }
            switch (sit) {
                case 0: {
                    if (fa == root && fa->NumChild == 2) {
                        EraseRoot();
                        //FuckNode(brother);
                        FuckNode(p);
                        return;
                    }
                    if (pos + 1 < fa->NumChild) MergeIndexPage(p, fa, pos);
                    else {
                        node *lbro = Getnode(fa->Children[pos - 1]);
                        MergeIndexPage(lbro, fa, pos - 1);
                        save_node(lbro);
                        FuckNode(p);
                        p=lbro;
                    }
                    bool flag=0;
                    if (fa->NumChild <= IndexSize / 2) { // fa->NumChild <= IndexSize / 2 ??
                        LendMergeIndex(fa);
                        flag=1;
                    }
                    if(!flag){
                        save_node(fa);
                        FuckNode(fa);
                    }
                    //save_node(brother);
                    save_node(p);
                    //FuckNode(brother);
                    FuckNode(p);
                    break;
                }
                case 1: {
                    /*
                     *  Lend from left brother TODO Debugging
                     */
                    node *grandson = Getnode(brother->Children[brother->NumChild - 1]);
                    grandson->father = p->pos;
                    for (int i = p->NumChild; i >= 1; i--) {
                        p->Children[i] = p->Children[i - 1];
                    }
                    p->Children[0] = grandson->pos;
                    for (int i = p->NumChild - 1; i >= 1; i--) {
                        p->data[i] = p->data[i - 1];
                    }
                    p->data[0] = grandson->data[0];
                    brother->NumChild--;
                    p->NumChild++;
                    fa->data[pos - 1] = p->data[0];
                    //int pos_p = fa->BinSearch(grandson->Children[0]->data.front().first);
                    save_node(grandson);
                    save_node(p);
                    save_node(fa);
                    save_node(brother);
                    FuckNode(fa);
                    FuckNode(grandson);
                    FuckNode(p);
                    FuckNode(brother);
                    break;
                }
                case 2: {
                    /*
                     *  Lend from right brother
                     */
                    //printf("11111111111111111111111\n");
                    node *grandson = Getnode(brother->Children[0]);
                    grandson->father = p->pos;
                    p->Children[p->NumChild] = grandson->pos;

                    p->data[p->NumChild - 1] = grandson->data[0];
                    p->NumChild++;
                    for (int i = 0; i < brother->NumChild - 2; i++) {
                        brother->data[i] = brother->data[i + 1];
                    }
                    for (int i = 0; i < brother->NumChild - 1; i++) {
                        brother->Children[i] = brother->Children[i + 1];
                    }
                    brother->NumChild--;
                    fa->data[pos] = brother->data[0];//!!!!!!!!!此处有更改 +1

                    save_node(p);
                    save_node(brother);
                    save_node(fa);
                    save_node(grandson);
                    FuckNode(fa);
                    FuckNode(p);
                    FuckNode(brother);
                    FuckNode(grandson);
                    break;
                }
            }
        }

        void MergeLeafPage(node* p, node* nxt, node* fa) {
            for (int i = p->NumChild; i < p->NumChild + nxt->NumChild; i++) {
                p->data[i] = nxt->data[i - p->NumChild];
            }
            p->NumChild += nxt->NumChild;
            int pos = fa->PosSearch(nxt->pos);
            for (int i = pos; i < fa->NumChild; i++) {
                fa->Children[i] = fa->Children[i + 1];
            }
            if(pos>=2)fa->data[pos - 2] = p->data[0];
            for (int i = pos-1; i < fa->NumChild - 2; i++) {
                fa->data[i] = fa->data[i + 1];
            }
            fa->NumChild--;
            if (tailLeaf == nxt->pos) {
                tailLeaf = p->pos;
                p->next = invalid_off;
            } else {
                p->next = nxt->next;
                node *nnt = Getnode(nxt->next);
                nnt->prev = p->pos;
                save_node(nnt);
                FuckNode(nnt);
            }
            save_node(p);
            save_node(fa);
            //FuckNode(nxt);
            //delete nxt;
        }

        /*
         * Merge index p with it's right brother
         */
        void MergeIndexPage(node* p, node* fa, int pos_p) {
            node *brother = Getnode(fa->Children[pos_p + 1]);
            for (int i = p->NumChild; i < p->NumChild + brother->NumChild; i++) {
                p->Children[i] = brother->Children[i - p->NumChild];
                node *tmp = Getnode(p->Children[i]);
                tmp->father = p->pos;
                //p->data[i - 1] = tmp->data[0];
                save_node(tmp);
                delete tmp;
            }
            p->data[p->NumChild - 1] = fa->data[pos_p];

            for (int i = p->NumChild; i < p->NumChild + brother->NumChild-1; i++) {
                p->data[i] = brother->data[i - p->NumChild];
            }

            p->NumChild += brother->NumChild;
            for (int i = pos_p + 1; i < fa->NumChild - 1; i++) {
                fa->Children[i] = fa->Children[i + 1];
                //fa->data[i - 1] = fa->data[i]; // TODO check correctness
            }
            for (int i = pos_p; i < fa->NumChild - 2; i++) {
                fa->data[i] = fa->data[i + 1];
            }
            fa->NumChild--;
            save_node(p);
            save_node(fa);
            FuckNode(brother);
        }

    public:
        typedef std::pair<Key, T> value_type;

        class node {
        public:
            int NumChild, type;
            off_t father, pos;
            off_t prev, next;
            value_type data[PageSize + 1];
            off_t Children[IndexSize + 1];

            node (const node& b){
                NumChild=b.NumChild;
                type=b.type;
                father=b.father;
                pos=b.pos;
                prev=b.prev;
                next=b.next;
                for(int i=0;i<=PageSize;i++)data[i]=b.data[i];
                for(int i=0;i<=IndexSize;i++)Children[i]=b.Children[i];
                ans++;
            }

            node(int t = 0) : type(t) {
                NumChild = 0;
                father = invalid_off;
                pos = invalid_off;
                prev = invalid_off;
                next = invalid_off;
                ans++;
            }
            ~node(){
                ans--;
            }

            bool Fewer(Key a, Key b, Compare C = Compare()) {
                return C(a, b);
            }

            bool Equal(Key a, Key b) {
                return !(Fewer(a, b) || Fewer(b, a));
            }

            void BinInsert(value_type value) {
                if (type == 1) {
                    data[NumChild] = value;
                    for (int i = NumChild; i >= 1; i--) {
                        if (data[i].first < data[i - 1].first) {
                            std::swap(data[i], data[i - 1]);
                        } else break;
                    }
                    NumChild++;
                }
            }

            void BinErase(Key key) {
                int pos;
                for (int i = 0; i <= NumChild; i++) {
                    if (data[i].first == key) {
                        pos = i;
                        break;
                    }
                }
                for (int i = pos; i < NumChild - 1; i++) {
                    data[i] = data[i + 1];
                }
                NumChild--;
            }

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

            int PosSearch(off_t pos) {
                for (int i = 0; i < NumChild; i++) {
                    if (Children[i] == pos) return i;
                }
                return -1;
            }

            void PrintLeafNode() {
                if (type != 1) throw (invalid_format());
                for (int i = 0; i < NumChild; i++) {
                    //std::cout << "Information for id " << data[i].first << " is " << data[i].second << "\n";
                }
            }

            void DeleteChild(int pos) {
                for (int i = pos; i < NumChild - 2; i++) {
                    data[i] = data[i + 1];
                }
                for (int i = pos + 1; i < NumChild - 1; i++) {
                    Children[i] = Children[i + 1];
                }
                NumChild--;
            }
        };

        class iterator {
            friend class Bptree;

        private:
            Bptree *bp;
            node *origin;
            int place;

        public:
            iterator() {
                bp = nullptr;
                origin = nullptr;
                place = 0;
            }

            iterator(node *p, Bptree *Bp, int t){
                if(p!=NULL)origin =new node(*p);
                else origin=NULL;
                place = t;
                bp = Bp;

            }

            iterator(const iterator &other) {
                if(other.origin!=NULL)origin =new node(*other.origin);
                else origin=NULL;
                place = other.place;
                bp = other.bp;
            }
            ~iterator(){
                if(origin!=NULL)delete origin;
            }
            iterator& operator=(const iterator &other) {
                if(origin!=NULL)delete origin;
                if(other.origin!=NULL)origin =new node(*other.origin);
                else origin=NULL;
                place = other.place;
                bp = other.bp;
                return *this;
            }



            /*
             *  iter++
             */
            iterator operator++(int) {
                iterator ans(*this);
                if (place == origin->NumChild - 1) {
                    if (origin->pos == bp->tailLeaf) {
                        place++;
                        return ans;
                    }
                    int ps = origin->next;
                    delete origin;
                    origin = Getnode(ps);
                    place = 0;
                } else {
                    place++;
                }

                return ans;
            }

            /*
             * ++iter
             */
            iterator &operator++() {
                if (place == origin->NumChild - 1) {
                    if (origin->pos == bp->tailLeaf) {
                        place++;
                        return *this;
                    }
                    int ps = origin->next;
                    delete origin;
                    origin = Getnode(ps);
                    place = 0;
                } else {
                    place++;
                }
                return *this;
            }

            /*
             *  iter--
             */
            iterator operator--(int) {
                iterator ans(*this);
                if (place == 0) {
                    if (origin->pos == bp->headLeaf) throw (invalid_iterator());
                    int ps = origin->prev;
                    delete origin;
                    origin = Getnode(ps);
                    place = origin->NumChild - 1;
                } else {
                    place--;
                }
                return ans;
            }

            /*
             *  --iter
             */
            iterator &operator--() {
                if (place == 0) {
                    if (origin->pos == bp->headLeaf) throw (invalid_iterator());
                    int ps = origin->prev;
                    delete origin;
                    origin = Getnode(ps);
                    place = origin->NumChild - 1;
                } else {
                    place--;
                }
                return *this;
            }

            value_type operator*() const {
                return origin->data[place];
            }

            value_type *operator->() const {
                value_type *ans = &origin->data[place];
                return ans;
            }

            bool operator==(const iterator &rhs) const {
                return ((origin->pos == rhs.origin->pos) && (place == rhs.place));
            }

            bool operator!=(const iterator &rhs) const {
                return ((origin->pos != rhs.origin->pos) || (place != rhs.place));
            }

            node *Getnode(off_t p) {
                if (p == invalid_off) throw (invalid_offset());
                bp->file.seekg(p);
                node *target = new node();
                bp->file.read(CAST(target), sizeof(node));
                return target;
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

        Bptree(const char *fname) {
            init();
            filename = new char[strlen(fname) + 1];
            strcpy(filename, fname);

            file.open(fname, std::fstream::out | std::fstream::in | std::fstream::binary);
            if (!file) {
                //std::cout << "file not exist...\n";
                CurrentLen = 0;
                file.open(fname, std::fstream::out| std::fstream::binary);
                //std::cout << "new file <" << fname << "> has been added to dir\n";
                file.close();
                file.open(fname, std::fstream::out | std::fstream::in | std::fstream::binary);
                save_main();
            } else {
                //std::cout << "File exist...\n";
                // std::cout << "Reading file...\n";
                read_info();
                //std::cout << "Initializing, we get " << CurrentLen << " data-pair currently." << "\n";
            }
        }

        ~Bptree() {
            save_main();
            if (root != nullptr) delete root;
            delete[] filename;
            file.close();
        }

        int size() {
            return CurrentLen;
        }

        bool empty() const {
            return (CurrentLen == 0);
        }

        void Modify(iterator iter, T newvalue) {
            iter.origin->data[iter.place].second = newvalue;
            save_node(iter.origin);
        }


        /*
         *  Return the iterator point to the first value-type whose id is key.
         */
        iterator find(const Key key) {
            off_t placeroot = root->pos;
            delete root;
            root = Getnode(placeroot);
            if (CurrentLen == 0) return end();
            node *p = Search(key);
            int flag = 1;
            while (p->pos != headLeaf) {
                if (Equal(key,p->data[0].first)) {
                    flag = 0;
                    node *tmp = Getnode(p->prev);
                    FuckNode(p);
                    p = tmp;
                } else {
                    break;
                }
            }
            if (flag) {
                for (int i = 0; i < p->NumChild; i++) {
                    if (p->data[i].first == key) {

                        iterator it(p, this, i);
                        FuckNode(p);
                        return it;
                    }
                }
                FuckNode(p);
                return end();
            } else {
                if (p->data[p->NumChild - 1].first != key) {
                    node *tmp = Getnode(p->next);
                    FuckNode(p);
                    if (tmp->data[0].first == key) {
                        iterator it(tmp, this, 0);
                        FuckNode(tmp);
                        return it;
                    }
                    FuckNode(tmp);
                    return end();
                }
                else {
                    for (int i = 0; i < p->NumChild; i++) {
                        if (p->data[i].first == key) {
                            iterator it(p, this, i);
                            FuckNode(p);
                            return it;
                        }
                    }
                    FuckNode(p);
                    return end();
                }
            }
        }

        iterator begin() {
            if (CurrentLen == 0) throw (container_is_empty());
            node *head = Getnode(headLeaf);
            iterator ans(head, this, 0);
            delete head;
            return ans;
        }

        iterator end() {
            node *end = Getnode(tailLeaf);
            iterator ans(end, this, end->NumChild);
            delete end;
            return ans;
        }

        node *Search(Key key) {
            if (CurrentLen == 0) return nullptr;
            node *p =root;
            int flag;
            while (p->type != 1) {
                flag = 0;
                for (int i = 0; i < p->NumChild - 1; i++) {
                    if (Fewer(key, p->data[i].first)) {
                        off_t nextp = p->Children[i];
                        FuckNode(p);
                        p = Getnode(nextp);
                        flag = 1;
                        break;
                    }
                }
                if (!flag) {
                    off_t nextp = p->Children[p->NumChild - 1];
                    FuckNode(p);
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
            // std::cout << "A new train is created, the train id is: " << value.first << ". ";
            // std::cout << "Train information is: " << value.second << "\n";
            if (CurrentLen == 1) {
                node *newleaf = NewNode(1);
                newleaf->NumChild++;
                newleaf->data[0] = value;
                save_node(newleaf);
                tailLeaf = newleaf->pos;
                headLeaf = newleaf->pos;
                //if(root!=NULL)delete root;
                root = newleaf;
                return;
            }

            node *p = Search(value.first);
            /*
             *  Case 1: the leaf page isn't full yet
             *          insert directly
             */
            if (p->NumChild + 1 < PageSize) {
                p->BinInsert(value);
                save_node(p);
                FuckNode(p);
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
                    node *fa=Father(p);
                    if (fa->NumChild < IndexSize) {
                        p->BinInsert(value);
                        SplitLeaf(p, fa);
                        /*
                         *  TODO : if p->prev not full, then rotate p
                         */
                        FuckNode(fa);
                        FuckNode(p);
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
                        FuckNode(p);
                        return;
                    }
                }


            }
        }

        void erase(Key key) {
            if (CurrentLen == 0) throw (container_is_empty());
            node *p = Search(key);
            if (p == nullptr) throw (invalid_iterator());
            CurrentLen--;
            if (p == root) {
                if (CurrentLen == 0) {
                    if (root != nullptr) {
                        root->NumChild--;
                        tailLeaf = invalid_off;
                        headLeaf = invalid_off;
                        save_node(root);
                    }
                    return;
                }
                p->BinErase(key);
                save_node(p);
                FuckNode(p);
                return;
            }

            node *fa = Father(p); // TODO : the problem is that fa might be changed...
            if (p->NumChild - 1 >= PageSize / 2) {
                /*
                 * Case 1: LeafPageSize > FillFactor;
                 *      Just erase directly(and update data of Indexpage)
                 */
                p->BinErase(key);
                int pos_p = fa->PosSearch(p->pos);
                if (pos_p>0&&fa->data[pos_p - 1].first != p->data[0].first) {
                    fa->data[pos_p - 1] = p->data[0];
                    save_node(fa);
                }
                save_node(fa);
                save_node(p);
                FuckNode(p);
                FuckNode(fa);
                return;
            } else {
                if (fa->NumChild - 1 > IndexSize / 2) {
                    /*
                     *  Case 2: else if IndexPageSize > FillFactor;
                     *      Get data from brother if brother is larger then fillfactor
                     *      else Merge Leaf p and its brother
                     */
                    p->BinErase(key);
                    LendMergeLeaf(p, fa);
                    FuckNode(p);
                    FuckNode(fa);
                } else {
                    /*
                     * Case 3: IndexPageSize & LeafPageSize <= FillFactor
                     *      Lend and merge recursively
                     */
                    p->BinErase(key);
                    //save_node(p);
                    bool flag=0;
                    if (!LendMergeLeaf(p, fa)) {
                        LendMergeIndex(fa);
                        save_node(p);
                        FuckNode(p);
                        //FuckNode(fa);
                        flag=1;
                    }
                    if(!flag){
                        save_node(p);
                        save_node(fa);
                        FuckNode(p);
                        FuckNode(fa);
                    }
                }

            }
        }


        void tranverse() {
            std::cout << "Tranversing information for all ids...\n";
            if (CurrentLen == 0) std::cout << "Bptree is empty now...\n Try inserting some datas.\n";
            else {
                node *p = Getnode(headLeaf);
                while (true) {
                    p->PrintLeafNode();
                    if (p->pos == tailLeaf) {
                        delete p;
                        break;
                    }
                    int pos = p->next;
                    delete p;
                    p = Getnode(pos);
                }
            }
        }

        /*
         * Return how many data is following key
         */
        int count(Key key) {
            iterator it;
            try{
                it = find(key);
            }
            catch(...){
                return 0;
            }
            int sum = 0;
            for (; it != end(); it++) {
                if ((*it).first != key) {
                    return sum;
                }
                sum++;
            }
            return sum;
        }

        /*
         *
         */
        void clear() {
            char newcommand[40] = "rm ";
            //  std::cout << "Removing files...\n";
            strncpy(newcommand + 3, filename, strlen(filename));
            system(newcommand);
        }


    };


}

#endif
