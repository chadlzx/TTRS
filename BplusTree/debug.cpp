//
// Created by yujie6 on 19-5-8.
//

#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

class node{
public:
    node * father, * child;
    int data;
    node(int value, node * fa = nullptr, node * child = nullptr):
    father(fa),
    child(child){
        data = value;
    }
//    ~node(){
//        if (father != nullptr) delete father;
//        if (child   != nullptr) delete child;
//    }
};

void change(node * &p);

int main(){
    node * p1 = new node (1);
    node * p2 = new node (2);

    p1->father = p2;
    p2->child = p1;
    change(p2);
    cout << p1->father->data << " " << p2->data;
    delete p1, p2;
    return 0;
}

void change(node * &p2){
    node * p3 = new node (3);
    p3->child = p2->child;
    p2->child->father = p3;

    cout << p3->child->data << " ";
}
