//
// Created by yujie6 on 19-5-8.
//

#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <deque>
using namespace std;
constexpr off_t invalid_off = 0xdeadbeef;

class store{
private:

    struct node {
        key_t key;
        off_t pos;
        off_t father;
        off_t prev;
        off_t next;
        size_t sz = 0;
        int type; //0 tree node 1 block
        node(key_t key = key_t(),
             off_t pos = invalid_off,
             off_t father = invalid_off,
             off_t prev = invalid_off,
             off_t next = invalid_off,
             size_t sz = 0,
             int type = 0)
                :key(key), pos(pos), father(father), prev(prev), next(next), sz(sz), type(type) {}
    };

    fstream file;
    char * filename;
    char * index_file;
    int CurrentLen = 0;
    off_t head, tail, root;

public:
    store(const char * fname, const char * index_fname)
    {
        filename = new char[strlen(fname) + 1];
        strcpy(filename, fname);
        index_file = new char[strlen(index_fname) + 1];
        strcpy(index_file, index_fname);

        file.open(fname, fstream::out|fstream::in|fstream::binary);
        if(!file) {
            cout << "file not exist...\n";
            CurrentLen = 0;
            file.open(fname, fstream::out|fstream::binary);
            cout << "new file <" << fname << "> has been added to dir\n";
        }
        else {
            cout << "file exist...\n";
            cout << "reading file...\n";
            read_info();
        }
    }

    ~store(){
        file.close();
    }

    void init(){

    }

    int size() {
        return CurrentLen;
    }

    void insert(int k){
        CurrentLen++;
        cout << "inserting...\n";
        node * bp = new node(k);
        file.seekp(0, ios::end);
        file.write(reinterpret_cast<const char *>(bp), sizeof(bp));
        delete bp;
        file.seekp(0);
        file.write(reinterpret_cast<const char *>(&CurrentLen), sizeof(int));
    }

    void read_info(){
        file.seekg(0);
        file.read(reinterpret_cast<char *>(&CurrentLen), sizeof(int));
    }
};


int main(){
    deque<int > a;
    cout << sizeof(deque<int>) << " " << sizeof(a) << " ";
    for (int i = 1; i <= 100; i++) a.push_back(i);
    cout << sizeof(a);
    return 0;
}


