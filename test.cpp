#include<bits/stdc++.h>
#include "NewBptree.hpp"
using namespace std;




template<int length>
struct String{
	char s[length];
	String(){memset(s,0,sizeof(s));}
	String(char* a){
		strcpy(s,a);
	}
	String& operator=(String b){
		strcpy(s,b.s);
	}
	String& operator=(char* b){
		strcpy(s,b);
	}
	String(const char* a){
		strcpy(s,a);
	}
	void print(){
		printf("%s",s);
	}
};


struct user{
	String<40> name;
	String<20> password;
	String<20> email;
	String<20> phone;
	int id,privilege;
};


sjtu::Bptree<int,user> USER("test.txt");

int main(){
	freopen("1.out","w",stdout);
	int n=100000;
	for(int i=1;i<=n;i++){
		user now;
		now.name="dahsabi";
		now.password="dashabi";
		now.email="dadashabi";
		now.phone="sb";
		now.id=i;now.privilege=1;
		USER.insert(std::pair<int,user>(now.id,now));//模拟注册
		
	}
	
	
	for(int i=1;i<=n;i++){
		user now=(*(USER.find(i))).second;//查找测试
		printf("%d\n",now.id);
	}
	
	
	
	for(int i=1;i<=n/2;i++){
		if(i==49281){
			cout<<"sb"<<endl;
		}
		user now=(*(USER.find(i))).second;
		now.name="chaojiwudidashabi";
		USER.erase(i);
		//USER.insert(std::pair<int,user>(now.id,now));//删除测试
		now=(*(USER.find(50065))).second;
		printf("%d %d %d\n",i,now.id,sjtu::ans);
	}
	/*
	for(int i=n/2+1;i<=n;i++){
		
		user now=(*(USER.find(i))).second;//查找测试
		printf("%d %d\n",now.id,sjtu::ans);
	
	}
	*/
	return 0;
	
	
		
}
