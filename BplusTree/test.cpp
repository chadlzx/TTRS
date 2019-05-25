#include <iostream>
#include <ctime>
#include <map>
#include "NewBptree.hpp"

using std::cout;

sjtu::Bptree<int, int> a("train_record");

void RobustTest() {
    int TestNum = 100;
    if (a.size() < TestNum) {
        for (int i = 3; i <= 3 + TestNum - 1; i++) {
            a.insert(std::pair<int, int>(i, 2 * i));
        }
    }
    for (int i = 12; i <= 69; i++) {
        a.erase(i);
        a.tranverse();
    }
    a.erase(70);
    a.tranverse();
}

void QueryTest() {
    cout << "Query test start: ";
    for (int i = 1; i <= 100; i++) {
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
    sjtu::Bptree<int, int>::iterator it = a.begin();
    for (; it != a.end(); it++) {
        cout << "Query information for train id " << (*it).first << ": it is " << (*it).second << "\n";
    }
}

void CountTest() {
    for (int i = 3; i <= 13; i++) {
        cout << "Count for train id " << i << ": it is " << a.count(i) << "\n";
    }
}


int main() {
    srand(time(NULL));
    clock_t start = clock();

    RobustTest();
    //RepeatIDTest();
    //CountTest();
    //IteratorTest();
    //QueryTest();

    cout << "Current length is " << a.size() << "\n";
    clock_t end = clock();
    cout << (double) (end - start) / (CLOCKS_PER_SEC) << "\n";

    a.clear();
    return 0;
}
