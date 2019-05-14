#include <iostream>
#include "Bptree.hpp"
#include "utility.hpp"
using std::cout;



int main(){
	sjtu::Bptree<int, int> a;
	/*
	 *  Insert test complete
	 *  Not correct... maintain data failed
	 *  Erase test complete 70% I guess
	 *
	 *  TODO: last time, I try to figure out when erase 5, there is a problem
	 *   when merging(forget root perhaps), later I found that data is not correct,
	 */
	for (int i = 5; i <= 30; i++) {
	    a.insert( std::pair<int, int>(i, 2 *i));
	}
	a.Print();
	/*for (int i = 5; i <= 15; i++) {
	    a.erase(i); //checking i = 5 now
	    a.Print();
	}*/



	return 0;
}
