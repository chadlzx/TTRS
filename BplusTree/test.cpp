#include <iostream>
#include "NewBptree.hpp"
using std::cout;



int main(){
	sjtu::bptree<int, int> a("train_record", "index_train_record");
	a.init();






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
