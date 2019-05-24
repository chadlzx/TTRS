#include <iostream>
#include <ctime>
#include "NewBptree.hpp"
using std::cout;


int main(){
    srand(time(NULL));
    sjtu::Bptree<int, int> a("train_record");
	int x;

	clock_t start= clock();


	int TestNum = 20;
    if (a.size() < TestNum) {
        for (int i = 3; i <= 3 + TestNum - 1 ; i++) {
            a.insert(std::pair<int, int>(i, 2 * i));
        }

        for (int i = 15; i <= 21; i++) {
            a.erase(i); //checking i = 5 now
            a.tranverse();
        }
    }
    else if (a.size() == TestNum){
        int flag2 = 0;
        if (flag2) {
            /*
             *  Insert test complete
             *  Not correct... maintain data failed
             *  Erase test complete 70% I guess
             *
             *  TODO: last time, I try to figure out when erase 5, there is a problem
             *   when merging(forget root perhaps), later I found that data is not correct,
             */
            for (int i = 3; i <= 4; i++) {
                a.erase(i); //checking i = 5 now
                a.tranverse();
            }
        }
    }
    else {
        a.clear();
    }

	//x = a.at(19);
	cout << "Current length is " << a.size() << "\n";
    clock_t end = clock();
    cout << (double) (end - start) / (CLOCKS_PER_SEC) << "\n";
	int flag = 0;
	if (flag) {
	    // Query test
	    cout << "Query test start: ";
        for (int i = 1; i <= 100; i++) {
            int id = rand() % (a.size()-20) + 10 ;
            cout << "Query information for train id " << id << ": it is " << a.at(id) << "\n";
        }
	}

	a.clear();
	return 0;
}
