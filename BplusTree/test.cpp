#include <iostream>
#include "Bptree.hpp"
#include "utility.hpp"
using std::cout;



int main(){
	sjtu::Bptree<int, int> a;
	/*for (int i = 5; i <= 19; i++) a.insert( std::pair<int, int>(i, 2 * i));
	a.Print();
	cout << a.size() << "\n";
	a.erase(19);
	a.erase(18);
	a.Print();*/
	//cout << a.at(16) << " ";

	for (int i = 5; i <= 19; i++) a.insert( std::pair<int, int>(i, 2 *i));
	a.Print();
	for (int i = 5; i <= 10; i++) {
	    a.erase(i); //checking i = 7 now
	    a.Print();
	}
	return 0;
}
