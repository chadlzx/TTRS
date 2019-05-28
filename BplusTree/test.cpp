#include <iostream>
#include <ctime>
#include <map>
#include "NewBptree.hpp"

using std::cout;

sjtu::Bptree<int, int, 101, 101> a("train_record");

void QueryTest();

void BigDataTest() {
    // 300000 with defult pagesize = 5 takes 9.81 seconds
    // 240000 with pagesize = 4096 takes about 13.06 seconds
    for (int i = 1; i <= 240000; i++) {
        a.insert(std::pair<int, int>(i, 2*i));
    }
    cout << "Query information for train id " << 100005 << ": it is " << a.at(100005) << "\n";
    //QueryTest();
}

void RobustTest() {
    int TestNum = 10;
    if (a.size() < TestNum) {
        for (int i = 3; i <= 3 + TestNum - 1; i++) {
            a.insert(std::pair<int, int>(i, 2 * i));
        }
    }
    for (int i = 12; i >= 3; i--) {
        a.erase(i);
        a.tranverse();
    }
    //a.erase(70);
    //a.tranverse();
}

void QueryTest() {
    cout << "Query test start: ";
    for (int i = 1; i <= 10000; i++) {
        int id = rand() % (a.size() - 20) + 10;
        cout << "Query information for train id " << id << ": it is " << a.at(id) << "\n";
    }
}

void RepeatIDTest() {
    for (int i = 3; i <= 1000; i++) {
        for (int j = 1; j <= 15; j++) {
            a.insert(std::pair<int, int>(i, i * j));
        }
    }
    a.tranverse();
    for (int id = 3; id <= 13; id++) {
        cout << "Query information for train id " << id << ": it is " << a.at(id) << "\n";
    }
}

void Map() {
    std::map<int, int> aa;
    for (int i = 3; i <= 13; i++) {
        for (int j = 1; j <= 5; j++) {
            aa.insert(std::pair<int, int>(i, i * j));
        }
    }
    for (int id = 3; id <= 13; id++) {
        cout << "Query information for train id " << id << ": it is " << aa.at(id) << "\n";
    }
}

void IteratorTest() {
    auto it = a.begin();
    for (; it != a.end(); it++) {
        cout << "Query information for train id " << it->first << ": it is " << it->second << "\n";
    }
}

void CountTest() {
    for (int i = 3; i <= 13; i++) {
        cout << "Count for train id " << i << ": it is " << a.count(i) << "\n";
    }
}

void FileTest() {
    if (a.size() == 0) {
        int TestNum = 1000;
        if (a.size() < TestNum) {
            for (int i = 3; i <= 3 + TestNum - 1; i++) {
                a.insert(std::pair<int, int>(i, 2 * i));
            }
        }
    }
    else {
        for (int j = 1; j <= 30; j++) {
            a.erase(a.size() + 2);
            for (int i = 3; i <= 3 + a.size() - 1; i++) {
                auto it = a.find(i);
                cout << "Query information for train id " << it->first << ": it is " << it->second << "\n";
            }
        }
    }
}


int main() {
    srand(time(NULL));
    clock_t start = clock();
    //BigDataTest();
    //RobustTest();
    //RepeatIDTest();
    //CountTest();
    //IteratorTest();
    //QueryTest();
    FileTest();


    cout << "Current length is " << a.size() << "\n";
    clock_t end = clock();
    cout << (double) (end - start) / (CLOCKS_PER_SEC) << "\n";

    //a.clear();
    return 0;
}
