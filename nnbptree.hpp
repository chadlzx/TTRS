//
// Created by yujie6 on 19-6-4.
//
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
    int ans = 0;
	template<class T>
	class shuzu{
		public:
		int len;
		char *filename;
        std::fstream file;
		shuzu(const char *fname){
            filename = new char[strlen(fname) + 1];
            strcpy(filename, fname);
            
        	file.open(fname, std::fstream::out | std::fstream::in | std::fstream::binary);
            if (!file) {
                len=0;
                file.open(fname, std::fstream::out | std::fstream::binary);
                file.close();
                file.open(fname, std::fstream::out | std::fstream::in | std::fstream::binary);
                save_main();
            } else {
                read_info();
            }
		}
		
		~shuzu(){
			save_main();
			delete[] filename;
            file.close();
		}
		void save_main(){
			file.seekp(0);
            file.write(CAST(&len), sizeof(int));
		}
		void read_info(){
			file.seekg(0);
            file.read(CAST(&len), sizeof(int));
		}
		T get(int k){
			file.seekg(sizeof(int)+(k-1)*sizeof(T));
			T now;
			file.read(CAST(&now),sizeof(T));
			return now;
		}
		int insert(T& b){
            file.seekp(0, std::ios::end);
            file.write(CAST(&b), sizeof(T));
			return ++len;
		}
		void change(int k,T& b){
			file.seekp(sizeof(int)+(k-1)*sizeof(T));
			file.write(CAST(&b), sizeof(T));
		}
		int size(){
			return len;
		}
	};
	
	
	
	
	
	
    template<
            class Key,
            class T,
            int IndexSize = 11,
            int PageSize = 11,
            class Compare = std::less<Key>
    >
    class Bptree {
    public:
        typedef std::pair<Key, T> value_type;

        class nnode;

        class indexnode;

        class leafnode;

        friend class iterator;

    private:
        off_t headLeaf, tailLeaf, posroot;
        int situation;
        nnode *root;
        int CurrentLen;
        char *filename;
        std::fstream file;

    public:

        inline void init() {
        	situation=0;root=NULL;CurrentLen=0;
            tailLeaf = invalid_off;
            headLeaf = invalid_off;
            posroot = invalid_off;
        }

        inline void read_info() {
            file.seekg(0);
            file.read(CAST(&situation), sizeof(int));
            file.seekg(sizeof(int));
            file.read(CAST(&posroot), sizeof(off_t));
            file.seekg(sizeof(int) + sizeof(off_t));
            file.read(CAST(&CurrentLen), sizeof(int));
            file.seekg(sizeof(off_t) +2* sizeof(int));
            file.read(CAST(&headLeaf), sizeof(off_t));
            file.seekg(2*sizeof(int) + 2 * sizeof(off_t));
            file.read(CAST(&tailLeaf), sizeof(off_t));
			if(situation==1)root=GetIndexnode(posroot);
			else root=GetLeafNode(posroot);
        }

        inline void save_main() {//将root的变量记录，会出错
            file.seekp(0);
            file.write(CAST(&situation), sizeof(int));
            file.seekp(sizeof(int));
            file.write(CAST(&posroot), sizeof(off_t));
            file.seekp(sizeof(int) + sizeof(off_t));
            file.write(CAST(&CurrentLen), sizeof(int));
            file.seekp(sizeof(off_t) + 2*sizeof(int));
            file.write(CAST(&headLeaf), sizeof(off_t));
            file.seekp(2*sizeof(int) + 2 * sizeof(off_t));
            file.write(CAST(&tailLeaf), sizeof(off_t));
        }

        leafnode *NewLeafNode() {
            leafnode *newleaf = new leafnode();
            file.seekp(0, std::ios::end);
            auto a1 = file.tellp();
            file.write(CAST(newleaf), sizeof(leafnode));
            file.seekp(0);
            auto a2 = file.tellp();
            newleaf->pos = off_t(a1 - a2);
            return newleaf;
        }

        indexnode *NewIndexNode() {
            indexnode *newleaf = new indexnode();
            file.seekp(0, std::ios::end);
            auto a1 = file.tellp();
            file.write(CAST(newleaf), sizeof(indexnode));
            file.seekp(0);
            auto a2 = file.tellp();
            newleaf->pos = off_t(a1 - a2);
            return newleaf;

        }

        indexnode *GetIndexnode(off_t p) {
            if (p == invalid_off) throw (invalid_offset());
            file.seekg(p);
            indexnode *target = new indexnode();
            file.read(CAST(target), sizeof(indexnode));
            return target;
        }

        leafnode *GetLeafNode(off_t p) {
            if (p == invalid_off) throw (invalid_offset());
            file.seekg(p);
            leafnode *target = new leafnode();
            file.read(CAST(target), sizeof(leafnode));
            return target;
        }

        void save_leaf_node(leafnode *p) {
            file.seekp(p->pos);
            file.write(CAST(p), sizeof(leafnode));
        }

        void save_index_node(indexnode *p) {
            file.seekp(p->pos);
            file.write(CAST(p), sizeof(indexnode));
        }

        void FuckIndexNode(indexnode *&p) {
            if (p != nullptr && p != root) {
                delete p;
                p = nullptr;
            }
        }

        void FuckLeafNode(leafnode *&p) {
            if (p != nullptr && p != root) {
                delete p;
                p = nullptr;
            }
        }

        inline indexnode *Father(nnode *p) { //no problem
            if (p->father == root->pos) {
                return (indexnode*)root;
            } else {
                return GetIndexnode(p->father);
            }
        }
		bool Fewer(Key a, Key b, Compare C = Compare()) {
            return C(a, b);
        }

        bool Equal(Key a, Key b) {
            return !(Fewer(a, b) || Fewer(b, a));
        }
		indexnode *GetBrother(indexnode *p) {//没有问题,自动认为p->NumChild为偶数
            indexnode *brother = NewIndexNode();
			brother->type=p->type;
            for (int i = IndexSize / 2 + 1; i <= IndexSize; i++) {
                brother->Children[i - IndexSize / 2 - 1] = p->Children[i];
                if(p->type==1){
					indexnode *pchild = GetIndexnode(p->Children[i]);
					pchild->father = brother->pos;
					save_index_node(pchild);
					delete pchild;
				}
				else {
					leafnode *pchild = GetLeafNode(p->Children[i]);
					pchild->father = brother->pos;
					save_leaf_node(pchild);
					delete pchild;
				}
            }
            brother->NumChild = IndexSize - IndexSize / 2;
            for (int i = IndexSize / 2 + 1; i < IndexSize; i++) {
                brother->data[i - (IndexSize / 2 + 1)] = p->data[i];
            }
            return brother;
        }

        void SplitLeafRoot(leafnode *p) {//检索完成
            leafnode *NewLeaf = NewLeafNode();
            for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                NewLeaf->data[i - PageSize / 2] = p->data[i];
            }
			NewLeaf->type=p->type;
            p->NumChild = PageSize / 2;
            NewLeaf->NumChild = PageSize - PageSize / 2;
            indexnode *fa = NewIndexNode();
            fa->NumChild = 2;
            fa->type = 0;
            fa->data[0] = NewLeaf->data[0].first;
            fa->Children[0] = p->pos;
            fa->Children[1] = NewLeaf->pos;
            p->father = fa->pos;
            NewLeaf->father = fa->pos;

            p->next = NewLeaf->pos;
            NewLeaf->prev = p->pos; //TODO p->next->prev = NewLeaf
            headLeaf = p->pos;
            tailLeaf = NewLeaf->pos;
            save_leaf_node(NewLeaf);
            save_index_node(fa);
            save_leaf_node(p);
            root = fa;
            situation = 1;
            FuckLeafNode(NewLeaf);
            FuckLeafNode(p);
        }
		void SplitIndexRoot(indexnode *p) {//必须为偶数 检查完毕
			assert((p->NumChild&1)==0);
            indexnode *fa = NewIndexNode();
			fa->type=1;
            fa->NumChild = 2;
            indexnode *brother = GetBrother(p);
			brother->type=p->type;
            p->NumChild = IndexSize / 2 + 1;
            fa->data[0] = p->data[IndexSize / 2];
            fa->Children[0] = p->pos;
            p->father = fa->pos;
            fa->Children[1] = brother->pos;
            brother->father = fa->pos;
			fa->type=1;
			
			situation=1;
            save_index_node(fa);
            save_index_node(p);
            save_index_node(brother);
            root = fa;
            FuckIndexNode(brother);
            FuckIndexNode(p);
        }
		void EraseRoot() {//检查完毕

           
            
			if (root->type == 0) {//合并两个leaf
				leafnode *child0 = GetLeafNode(((indexnode*)root)->Children[0]);
                leafnode *bro = GetLeafNode(((indexnode*)root)->Children[1]);

                for (int i = 0; i < bro->NumChild; i++) 
                    child0->data[child0->NumChild + i] = bro->data[i];
				child0->father=invalid_off;//?
                child0->NumChild += bro->NumChild;
                child0->type=-1;
                delete root;
                root=child0;
                delete bro;
				situation=0;
                save_leaf_node((leafnode*)root);
                return;
            }
			indexnode *child0 = GetIndexnode(((indexnode*)root)->Children[0]);
            indexnode *bro = GetIndexnode(((indexnode*)root)->Children[1]);
            child0->father = invalid_off;

            for (int i = child0->NumChild ; i < child0->NumChild + bro->NumChild; i++) {
                child0->Children[i] = bro->Children[i - child0->NumChild];
                
                if(child0->type==0){//tmp为leaf
					leafnode *tmp = GetLeafNode(bro->Children[i- child0->NumChild]);//cnm
					tmp->father = child0->pos;
					save_leaf_node(tmp);
					delete tmp;
				}
				else {
					indexnode *tmp = GetIndexnode(bro->Children[i- child0->NumChild]);//cnm
					tmp->father = child0->pos;
					save_index_node(tmp);
					delete tmp;
				}
            }

            for (int i = child0->NumChild; i < child0->NumChild + bro->NumChild - 1; i++) {
                child0->data[i] = bro->data[i- child0->NumChild];
            }

            child0->data[child0->NumChild-1] = ((indexnode*)root)->data[0];
            child0->NumChild += bro->NumChild;
            save_index_node(child0);
            delete bro;
            delete root;
			situation=1;
            root = child0;
        }
		void SplitIndex(indexnode *p) {
            /*
             *  Case 1: p is not root
             *     do it recursively;
             */
            while (p != root) {//检查完毕
                if (p->NumChild > IndexSize) {
                    indexnode *brother = GetBrother(p);
                    indexnode *fa = Father(p);
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
                    save_index_node(brother);
                    save_index_node(fa);
                    save_index_node(p);
                    FuckIndexNode(p);
                    FuckIndexNode(brother);
                    p = fa;
                } else break;
            }
            if(p!=(indexnode*)root&&p->NumChild <= IndexSize){
            	delete p;
            	return ;
            }
            if (p == (indexnode*)root && p->NumChild > IndexSize) {
                SplitIndexRoot(p);
            }
		}
		
        void SplitLeaf(leafnode* p, indexnode *fa) {//一检测
            leafnode *NewLeaf = NewLeafNode();
            for (int i = PageSize / 2; i <= PageSize - 1; i++) {
                NewLeaf->data[i - PageSize / 2] = p->data[i];
            }
            p->NumChild = PageSize / 2;
            NewLeaf->NumChild = PageSize - PageSize / 2;
            NewLeaf->father = fa->pos;
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
            fa->Children[pos_p + 1] = NewLeaf->pos;

            for (int i = fa->NumChild - 1; i > pos_p; i--) {
                fa->data[i] = fa->data[i-1];
            }
            fa->data[pos_p] = NewLeaf->data[0].first; //TODO
			
			fa->type=0;
            fa->NumChild++;
            off_t PosNextp = p->next;
            if (p->pos == tailLeaf) {
                tailLeaf = NewLeaf->pos;
            }
            p->next = NewLeaf->pos;
            NewLeaf->prev = p->pos;

            if (PosNextp != invalid_off) {
                leafnode *nextp = GetLeafNode(PosNextp);
                nextp->prev = NewLeaf->pos;
                NewLeaf->next = nextp->pos;
                save_leaf_node(nextp);
                FuckLeafNode(nextp);
            }

            save_index_node(fa);
            save_leaf_node(p);
            save_leaf_node(NewLeaf);
            FuckLeafNode(NewLeaf);
        }
		bool LendMergeLeaf(leafnode*& p, indexnode* fa) {
            int sit = 3;
            leafnode *pre = nullptr, *nxt = nullptr;
            if (p->prev != invalid_off) {
                pre = GetLeafNode(p->prev);
                if (pre->NumChild > PageSize / 2) sit = 1;
            }
            if (sit == 3) {
                if (p->next != invalid_off) {
                    nxt = GetLeafNode(p->next);
                    if (nxt->NumChild > PageSize / 2) sit = 2;

                }
            }
            switch (sit) {
                case 1: {
                    p->BinInsert(pre->data[pre->NumChild - 1]);
                    pre->NumChild--;
                    save_leaf_node(pre);
                    save_leaf_node(p);
                    FuckLeafNode(pre);
                    return true;
                }
                case 2: {
                    p->BinInsert(nxt->data[0]);
                    int pos = fa->BinSearch(nxt->data[0].first);
                    for (int i = 0; i < nxt->NumChild - 1; i++) {
                        nxt->data[i] = nxt->data[i + 1];
                    }
                    if(pos)fa->data[pos - 1] = p->data[0].first;
                    fa->data[pos] = nxt->data[0].first;
                    nxt->NumChild--;
                    save_leaf_node(nxt);
                    save_leaf_node(p);
                    save_index_node(fa);
                    FuckLeafNode(pre);
                    FuckLeafNode(nxt);
                    return true;
                }
                case 3: {
                    int pos_p = fa->PosSearch(p->pos);
                    if (p->next != invalid_off && pos_p != fa->NumChild - 1) {
                        MergeLeafPage(p, nxt, fa);
                        FuckLeafNode(pre);
                        FuckLeafNode(nxt);
                    }
                    else {
                        MergeLeafPage(pre, p, fa);
                        FuckLeafNode(nxt);
                        save_leaf_node(pre);
                        FuckLeafNode(p);
                        p=pre;
                    }
                    return false;
                }
            }
        }
		
		void LendMergeIndex(indexnode*  p) {//p自行處理
            if (p == root) {
                /*
                 *  Special case: leaf page is right under root
                 */
                if (p->NumChild == 1) {
					if(p->type==1){
						root = GetIndexnode(p->Children[0]);
						FuckIndexNode(p);
						root->father = invalid_off;
						save_index_node((indexnode*)root);
						situation=1;
                    }
					else {
						root = GetLeafNode(p->Children[0]);
						FuckIndexNode(p);
						root->father = invalid_off;
						save_leaf_node((leafnode*)root);
						situation=0;
					}
					
					return;
                }
                if (p->NumChild != 2) return; //Pagesize / 2 ???
                else {
					if(p->type==1){
						indexnode *child0 = GetIndexnode(p->Children[0]);
						indexnode *child1 = GetIndexnode(p->Children[1]);
						if (child0->NumChild == IndexSize / 2 && child1->NumChild == IndexSize / 2) {
							EraseRoot();
						}
						FuckIndexNode(child0);
						FuckIndexNode(child1);
					}
					else {
						leafnode *child0 = GetLeafNode(p->Children[0]);
						leafnode *child1 = GetLeafNode(p->Children[1]);
						if (child0->NumChild == IndexSize / 2 && child1->NumChild == IndexSize / 2) {
							EraseRoot();
						}
						FuckLeafNode(child0);
						FuckLeafNode(child1);
					}
                }
                return;
            }
            indexnode *fa = Father(p);
            indexnode *brother = nullptr;
            //if(fa==root){
                //printf("嘿嘿嘿\n");
            //}
            int sit = 0;
            int pos = fa->PosSearch(p->pos);
            if (pos >= 1) {
                brother = GetIndexnode(fa->Children[pos - 1]);
                if (brother->NumChild - 1 > IndexSize / 2) sit = 1;
                else FuckIndexNode(brother);
            }
            if (!sit) {
                if (pos + 1 < fa->NumChild) {
                    brother = GetIndexnode(fa->Children[pos + 1]);
                    if (brother->NumChild - 1 > IndexSize / 2) sit = 2;
                    else FuckIndexNode(brother);
                }
            }
            switch (sit) {
                case 0: {
                    if (fa == root && fa->NumChild == 2) {
                        EraseRoot();
                        //FuckNode(brother);
                        FuckIndexNode(p);
                        return;
                    }
                    if (pos + 1 < fa->NumChild) MergeIndexPage(p, fa, pos);
                    else {
                        indexnode *lbro = GetIndexnode(fa->Children[pos - 1]);
                        MergeIndexPage(lbro, fa, pos - 1);
                        save_index_node(lbro);
                        FuckIndexNode(p);
                        p=lbro;
                    }
                    bool flag=0;
                    if (fa->NumChild <= IndexSize / 2) { // fa->NumChild <= IndexSize / 2 ??
                        LendMergeIndex(fa);
                        flag=1;
                    }
                    if(!flag){
                        save_index_node(fa);
                        FuckIndexNode(fa);
                    }
                    //save_node(brother);
                    save_index_node(p);
                    //FuckNode(brother);
                    FuckIndexNode(p);
                    break;
                }
                case 1: {
                    /*
                     *  Lend from left brother TODO Debugging
                     */
					if(brother->type==1){
						indexnode *grandson = GetIndexnode(brother->Children[brother->NumChild - 1]);
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
						save_index_node(grandson);
						save_index_node(p);
						save_index_node(fa);
						save_index_node(brother);
						FuckIndexNode(fa);
						FuckIndexNode(grandson);
						FuckIndexNode(p);
						FuckIndexNode(brother);
					}
					else {
						leafnode *grandson = GetLeafNode(brother->Children[brother->NumChild - 1]);
						grandson->father = p->pos;
						for (int i = p->NumChild; i >= 1; i--) {
							p->Children[i] = p->Children[i - 1];
						}
						p->Children[0] = grandson->pos;
						for (int i = p->NumChild - 1; i >= 1; i--) {
							p->data[i] = p->data[i - 1];
						}
						p->data[0] = grandson->data[0].first;
						brother->NumChild--;
						p->NumChild++;
						fa->data[pos - 1] = p->data[0];
						//int pos_p = fa->BinSearch(grandson->Children[0]->data.front().first);
						save_leaf_node(grandson);
						save_index_node(p);
						save_index_node(fa);
						save_index_node(brother);
						FuckIndexNode(fa);
						FuckLeafNode(grandson);
						FuckIndexNode(p);
						FuckIndexNode(brother);
					}
                    break;
                }
                case 2: {
                    /*
                     *  Lend from right brother
                     */
                    //printf("11111111111111111111111\n");
					if(brother->type==1){
						indexnode *grandson = GetIndexnode(brother->Children[0]);
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
						fa->data[pos] = brother->data[0];

						save_index_node(p);
						save_index_node(brother);
						save_index_node(fa);
						save_index_node(grandson);
						FuckIndexNode(fa);
						FuckIndexNode(p);
						FuckIndexNode(brother);
						FuckIndexNode(grandson);
					}
					else{
						leafnode *grandson = GetLeafNode(brother->Children[0]);
						grandson->father = p->pos;
						p->Children[p->NumChild] = grandson->pos;

						p->data[p->NumChild - 1] = grandson->data[0].first;
						p->NumChild++;
						for (int i = 0; i < brother->NumChild - 2; i++) {
							brother->data[i] = brother->data[i + 1];
						}
						for (int i = 0; i < brother->NumChild - 1; i++) {
							brother->Children[i] = brother->Children[i + 1];
						}
						brother->NumChild--;
						fa->data[pos] = brother->data[0];

						save_index_node(p);
						save_index_node(brother);
						save_index_node(fa);
						save_leaf_node(grandson);
						FuckIndexNode(fa);
						FuckIndexNode(p);
						FuckIndexNode(brother);
						FuckLeafNode(grandson);
					}
                    break;
                }
            }
        }
		
		void MergeLeafPage(leafnode* p, leafnode* nxt, indexnode* fa) {
            for (int i = p->NumChild; i < p->NumChild + nxt->NumChild; i++) {
                p->data[i] = nxt->data[i - p->NumChild];
            }
            p->NumChild += nxt->NumChild;
            int pos = fa->PosSearch(nxt->pos);
            for (int i = pos; i < fa->NumChild; i++) {
                fa->Children[i] = fa->Children[i + 1];
            }
            if(pos>=2)fa->data[pos - 2] = p->data[0].first;
            for (int i = pos-1; i < fa->NumChild - 2; i++) {
                fa->data[i] = fa->data[i + 1];
            }
            fa->NumChild--;
            if (tailLeaf == nxt->pos) {
                tailLeaf = p->pos;
                p->next = invalid_off;
            } else {
                p->next = nxt->next;
                leafnode *nnt = GetLeafNode(nxt->next);
                nnt->prev = p->pos;
                save_leaf_node(nnt);
                FuckLeafNode(nnt);
            }
            save_leaf_node(p);
            save_index_node(fa);
            //FuckNode(nxt);
            //delete nxt;
        }

        /*
         * Merge index p with it's right brother
         */
        void MergeIndexPage(indexnode* p, indexnode* fa, int pos_p) {
            indexnode *brother = GetIndexnode(fa->Children[pos_p + 1]);
            for (int i = p->NumChild; i < p->NumChild + brother->NumChild; i++) {
                p->Children[i] = brother->Children[i - p->NumChild];
				if(p->type==1){
					indexnode *tmp = GetIndexnode(p->Children[i]);
					tmp->father = p->pos;
					//p->data[i - 1] = tmp->data[0];
					save_index_node(tmp);
					delete tmp;
				}
				else {
					leafnode *tmp = GetLeafNode(p->Children[i]);
					tmp->father = p->pos;
					//p->data[i - 1] = tmp->data[0];
					save_leaf_node(tmp);
					delete tmp;
				}
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
            save_index_node(p);
            save_index_node(fa);
            FuckIndexNode(brother);
        }

		
		
		
		
        class nnode {
        public:
            int type, NumChild;
            off_t father, pos;

            nnode() {
                father = invalid_off;
                pos = invalid_off;
                NumChild = 0;
            }

            ~nnode() {
                ans--;
            }

            
        };

        class leafnode : public nnode {
        public:
            value_type data[PageSize + 1];
            off_t prev, next;

            leafnode() : nnode() {
                prev = invalid_off;
                next = invalid_off;
                nnode::type = -1;
            }

            leafnode(const leafnode &b) : nnode() {
                nnode::pos = b.pos;
                nnode::father = b.father;
                nnode::NumChild = b.NumChild;
                next = b.next;
                prev = b.prev;
                for (int i = 0; i < nnode::NumChild; i++) {
                    data[i] = b.data[i];
                }
            }

            void BinInsert(value_type value) {
                data[nnode::NumChild] = value;
                for (int i = nnode::NumChild; i >= 1; i--) {
                    if (data[i].first < data[i - 1].first) {
                        std::swap(data[i], data[i - 1]);
                    } else break;
                }
                nnode::NumChild++;
            }

            int BinSearch(Key key) {
                for (int i = 0; i < nnode::NumChild; i++) {
                    if (nnode::Equal(key, data[i].first)) return i;
                }
                return -1;
            }

            void BinErase(Key key) {
                int pos;
                for (int i = 0; i <= nnode::NumChild; i++) {
                    if (data[i].first == key) {
                        pos = i;
                        break;
                    }
                }
                for (int i = pos; i < nnode::NumChild - 1; i++) {
                    data[i] = data[i + 1];
                }
                nnode::NumChild--;
            }
        };

        class indexnode : public nnode {
        public:
            off_t Children[IndexSize + 1];
            Key data[PageSize + 1];

            indexnode() {
                nnode::type = 0;
                ans++;
            }

            indexnode(const indexnode &b) {
                nnode::pos = b.pos;
                nnode::father = b.father;
                nnode::NumChild = b.NumChild;
                nnode::type = b.type;
                for (int i = 0; i < nnode::NumChild - 1; i++) {
                    data[i] = b.data[i];
                }
                for (int i = 0; i < nnode::NumChild; i++) {
                    Children[i] = b.Children[i];
                }
            }

            int PosSearch(off_t pos) {
                for (int i = 0; i < nnode::NumChild; i++) {
                    if (Children[i] == pos) return i;
                }
                return -1;
            }

            int BinSearch(Key key) {
                for (int i = 0; i < nnode::NumChild - 1; i++) {
                    if (key==data[i]) return i;
                }
                return -1;
            }
        };

        class iterator {
            friend class Bptree;

        private:
            Bptree *bp;
            leafnode *origin;
            int place;

        public:
            iterator() {
                bp = nullptr;
                origin = nullptr;
                place = invalid_off;
            }

            iterator(leafnode *p, Bptree *Bp, int t) {
                if (p != NULL)origin = new leafnode(*p);
                else origin = NULL;
                place = t;
                bp = Bp;

            }

            iterator(const iterator &other) {
                if (other.origin != NULL) origin = new leafnode(*other.origin);
                else origin = NULL;
                place = other.place;
                bp = other.bp;
            }

            ~iterator() {
                if (origin != NULL)delete origin;
            }

            iterator &operator=(const iterator &other) {
                if (origin != NULL)delete origin;
                if (other.origin != NULL)origin = new leafnode(*other.origin);
                else origin = NULL;
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
                    origin = bp->GetLeafNode(ps);
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
                    origin = bp->GetLeafNode(ps);
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
                    origin = bp->GetLeafNode(ps);
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
                    origin = bp->GetLeafNode(ps);
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


        };


        iterator begin() {
            if (CurrentLen == 0) throw (container_is_empty());
            leafnode *head = GetLeafNode(headLeaf);
            iterator ans(head, this, 0);
            delete head;
            return ans;
        }

        iterator end() {
            leafnode *end = GetLeafNode(tailLeaf);
            iterator ans(end, this, end->NumChild);
            delete end;
            return ans;
        }

        Bptree(const char *fname) {
            init();
            filename = new char[strlen(fname) + 1];
            strcpy(filename, fname);

            file.open(fname, std::fstream::out | std::fstream::in | std::fstream::binary);
            if (!file) {
                CurrentLen = 0;
                situation = 0;
                file.open(fname, std::fstream::out | std::fstream::binary);
                file.close();
                file.open(fname, std::fstream::out | std::fstream::in | std::fstream::binary);
                save_main();
            } else {
                read_info();
            }
        }


        ~Bptree() {
			posroot=root->pos;
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
            save_leaf_node(iter.origin);
        }

        iterator find(Key key) {
            off_t placeroot = root->pos;delete root;
            if(situation)root = GetIndexnode(placeroot);
			else root= GetLeafNode(placeroot);
			
            if (CurrentLen == 0) return end();
            leafnode *p = Search(key);
            int flag = 1;
            while (p->pos != headLeaf) {
                if (Equal(key,p->data[0].first)) {
                    flag = 0;
                    leafnode *tmp = GetLeafNode(p->prev);
                    FuckLeafNode(p);
                    p = tmp;
                } else {
                    break;
                }
            }
            if (flag) {
                for (int i = 0; i < p->NumChild; i++) {
                    if (p->data[i].first == key) {

                        iterator it(p, this, i);
                        FuckLeafNode(p);
                        return it;
                    }
                }
                FuckLeafNode(p);
                return end();
            } else {
                if (p->data[p->NumChild - 1].first != key) {
                    leafnode *tmp = GetLeafNode(p->next);
                    FuckLeafNode(p);
                    if (tmp->data[0].first == key) {
                        iterator it(tmp, this, 0);
                        FuckLeafNode(tmp);
                        return it;
                    }
                    FuckLeafNode(tmp);
                    return end();
                }
                else {
                    for (int i = 0; i < p->NumChild; i++) {
                        if (p->data[i].first == key) {
                            iterator it(p, this, i);
                            FuckLeafNode(p);
                            return it;
                        }
                    }
                    FuckLeafNode(p);
                    return end();
                }
            }
			return end();
        }

        leafnode *Search(Key key) {
            if (CurrentLen == 0) return nullptr;
            if (situation == 0) {
                return reinterpret_cast<leafnode *>(root);
            } else {
                indexnode *p = reinterpret_cast<indexnode *>(root);
                leafnode * ans;
                int flag;
                while (p->type != -1) {
                    flag = 0;
                    for (int i = 0; i < p->NumChild - 1; i++) {
                        if (Fewer(key, p->data[i])) {
                            off_t nextp = p->Children[i];
                            if (p->type== 1) {
                            	indexnode* next= GetIndexnode(nextp);
                            	next->father = p->pos;
                            	 FuckIndexNode(p);
                            	 p=next;save_index_node(p);
                            }
                            else {
                            	
                                ans = GetLeafNode(nextp);
                                ans->father=p->pos;          
                                FuckIndexNode(p);return ans;
                            }
                            flag = 1;
                            break;
                        }
                    }
                    if (!flag) {
                        off_t nextp = p->Children[p->NumChild - 1];
                        if (p->type== 1) {
                            indexnode* next= GetIndexnode(nextp);
                            next->father = p->pos;
                            FuckIndexNode(p);
                            p=next;
                        }
                        else {
                            ans = GetLeafNode(nextp);
                            ans->father=p->pos;          
                            FuckIndexNode(p);return ans;
                        }
                    }
                }
				return NULL;
            }
			return NULL;
        }


        void insert(value_type value) {
            CurrentLen++;

            if (CurrentLen == 1) {
                leafnode *newleaf = NewLeafNode();
                newleaf->NumChild++;
                newleaf->data[0] = value;
                save_leaf_node(newleaf);
                tailLeaf = newleaf->pos;
                headLeaf = newleaf->pos;
                root = newleaf;
                situation = 0;
                posroot = newleaf->pos;
                return;
            }

            leafnode *p = Search(value.first);

            if (p->NumChild + 1 < PageSize) {
                p->BinInsert(value);
                save_leaf_node(p);
                FuckLeafNode(p);
                return;
            } else {
                if (p == root) {
                    p->BinInsert(value);
                    SplitLeafRoot(p);
                    return;
                } else {
                    indexnode *fa=Father(p);
                    if (fa->NumChild < IndexSize) {
                        p->BinInsert(value);
                        SplitLeaf(p, fa);
                        /*
                         *  TODO : if p->prev not full, then rotate p
                         */
                        FuckIndexNode(fa);
                        FuckLeafNode(p);
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
                        FuckLeafNode(p);
                        return;
                    }
                }
            }
        }
		void erase(Key key) {
            if (CurrentLen == 0) throw (container_is_empty());
            leafnode *p = Search(key);
            if (p == nullptr) throw (invalid_iterator());
            CurrentLen--;
            if (p == root) {
                if (CurrentLen == 0) {
                    if (root != nullptr) {
                        delete root;root=NULL;
                        tailLeaf = invalid_off;
                        headLeaf = invalid_off;
                    }
                    return;
                }
                p->BinErase(key);
                save_leaf_node(p);
                FuckLeafNode(p);
                return;
            }

            indexnode *fa = Father(p); // TODO : the problem is that fa might be changed...
            if (p->NumChild - 1 >= PageSize / 2) {
                /*
                 * Case 1: LeafPageSize > FillFactor;
                 *      Just erase directly(and update data of Indexpage)
                 */
                p->BinErase(key);
                int pos_p = fa->PosSearch(p->pos);
                if (pos_p>0&&fa->data[pos_p - 1] != p->data[0].first) {
                    fa->data[pos_p - 1] = p->data[0].first;
                    save_index_node(fa);
                }
                save_index_node(fa);
                save_leaf_node(p);
                FuckLeafNode(p);
                FuckIndexNode(fa);
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
                    FuckLeafNode(p);
                    FuckIndexNode(fa);
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
                        save_leaf_node(p);
                        FuckLeafNode(p);
                        //FuckNode(fa);
                        flag=1;
                    }
                    if(!flag){
                        save_leaf_node(p);
                        save_index_node(fa);
                        FuckLeafNode(p);
                        FuckIndexNode(fa);
                    }
                }

            }
        }
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
