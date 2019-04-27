#include <iostream>
#include "Bptree.hpp"
#include "utility.hpp"
using std::cout;



int main(){
	sjtu::Bptree<int, int> a;
	for (int i = 1; i <= 12; i++) a.insert( sjtu::pair<int, int>(i, 2 * i));
	cout << a.size() << "\n";
    cout << a.at(4) << " ";
	cout << a.at(12);
	return 0;
}

