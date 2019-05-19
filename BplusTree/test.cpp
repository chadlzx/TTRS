#include <iostream>
#include <ctime>
#include "NewBptree.hpp"
using std::cout;


int main(){
    srand(time(NULL));
    sjtu::Bptree<int, int> a("train_record");
	int x;
	int TestNum = 15060;
    if (a.size() < TestNum) {
        for (int i = 3; i <= 3 + TestNum - 1 ; i++) {
            a.insert(std::pair<int, int>(i, 2 * i));
        }
    }
    else if (a.size() == TestNum){
        a.insert(std::pair<int, int> (a.size()+3, 2*(a.size() + 3) ) );
    }

	x = a.at(a.size() + 1);

	cout << "Current length is " << a.size() << "\n";
	int flag = 1;
	if (flag) {
	    // Query test
	    cout << "Query test start: ";
        for (int i = 1; i <= 100; i++) {
            int id = rand() % (a.size()-20) + 10 ;
            cout << "Query information for train id " << id << ": it is " << a.at(id) << "\n";
        }
	}





	/*
	 *  Insert test complete
	 *  Not correct... maintain data failed
	 *  Erase test complete 70% I guess
	 *
	 *  TODO: last time, I try to figure out when erase 5, there is a problem
	 *   when merging(forget root perhaps), later I found that data is not correct,
	 */



	/*
	 * for (int i = 5; i <= 30; i++) {
	    a.insert( std::pair<int, int>(i, 2 *i));
	}
	a.Print();
	 *
	 * /
	/*
	for (int i = 5; i <= 15; i++) {
	    a.erase(i); //checking i = 5 now
	    a.Print();
	}
	 */



	return 0;
}
