#include<cmath>

#include<iostream>

#include<cstdio>

#include<cstdlib>



#include "NewBptree.hpp"

using namespace std;

using namespace sjtu;



const int inf=1e9;

template<int> struct String;

struct user;

struct Station;

struct ticket;

struct train;

struct KEY2;





//--------------------------------------------------------------------------

//以下为用户相关内容

//--------------------------------------------------------------------------

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

	bool operator<(String b)const{

		return strcmp(s,b.s)<0;

	}

	bool operator==(String b)const{

		return strcmp(s,b.s)==0;

	}

	bool operator>(String b)const{

		return strcmp(s,b.s)>0;

	}

	bool operator!=(String b)const{

		return !((*this)==b);

	}

	

	template<int lengt>

	friend std::ostream & operator << (std::ostream &out,String<lengt> s);

	

	void print(){

		printf("%s",s);

	}

	int trans(){//将XX:XX转化为秒数

		if(s[2]==':'){

			int shi=((s[0]-'0')*10+s[1]-'0')*60;

			int fen=((s[3]-'0')*10+s[4]-'0');

			fen+=shi;

			return fen;

		}

		else return -1;

	}

	int trans2(){

		return (s[8]-'0')*10+s[9]-'0';

	}

	

	bool beibaohan(String b){

		int m=strlen(b.s);

		for(int i=0;i<m;i++)

			if(b.s[i]==s[0])return 1;

		return 0;

	}

};

template<int lengt>

std::ostream & operator << (std::ostream &out,String<lengt> s)

{

	s.print();

	return out;

}



struct user{

	String<40> name;

	String<20> password;

	String<20> email;

	String<20> phone;

	int id,privilege;

	user(String<40> Name="",String<20> Password="",String<20> Email="",String<20> Phone=""){

		name=Name;

		password=Password;

		email=Email;

		phone=Phone;

		privilege=1;id=0;

	}

	void print(){

		name.print();printf(" ");

		email.print();printf(" ");

		phone.print();printf(" ");

		printf("%d\n",privilege);

	}

	void change(String<40> Name,String<20> Password,String<20> Email,String<20> Phone){

		name=Name; 

		password=Password;

		email=Email;

		phone=Phone;

	}

};

struct Station{

	String<40> name;

	String<6> arrive_time;

	String<6> start_time;

	String<6> stopover_time;

	double price[6];int num;

	String<40> name_price[6];

	int num_remain[31][6];

	Station(){

		num=0;

		for(int j=1;j<=30;j++)

			for(int i=0;i<6;i++)

				price[i]=0,num_remain[j][i]=2000;

		

	}

	Station& operator=(const Station& b){

		name=b.name;

		arrive_time=b.arrive_time;

		start_time=b.start_time;

		stopover_time=b.stopover_time;

		num=b.num;

		for(int i=0;i<num;i++)price[i]=b.price[i];

		for(int i=0;i<num;i++)name_price[i]=b.name_price[i];

		for(int i=1;i<=30;i++)

			for(int j=0;j<num;j++)

				num_remain[i][j]=b.num_remain[i][j];

		return *this;

	}

	void print(){

		name.print();printf(" ");

		arrive_time.print();printf(" ");

		start_time.print();printf(" ");

		stopover_time.print();printf(" ");

		

		for(int i=0;i<num;i++)printf("￥%.2lf%c",price[i],i==num-1?'\n':' ');

	}

	void input(int numm,String<40> Name_price[]){

		char op[41];

		scanf("%s",op);name=op;

		scanf("%s",op);arrive_time=op;

		scanf("%s",op);start_time=op;

		scanf("%s",op);stopover_time=op;

		for(int i=0;i<numm;i++)scanf(" ￥%lf",&price[i]);

		for(int j=1;j<=30;j++)

			for(int i=0;i<numm;i++)

				num_remain[j][i]=2000;

		for(int i=0;i<numm;i++)name_price[i]=Name_price[i];

		num=numm;

	}

};





struct train{

	String<20> id;

	String<40> name;

	String<10> catalog;

	int num_price;

	String<40> name_price[6];

	int num_station;

	Station station[60];

	bool issale;

	train(){

		issale=0;

	}

	train(String<20> Id,String<40> Name,String<10> Catalog,int Num_station,

		int Num_price,String<40> Name_price[],Station SStation[]){

		id=Id;

		name=Name;

		catalog=Catalog;

		num_station=Num_station;

		num_price=Num_price;

		for(int i=0;i<num_station;i++)station[i]=SStation[i];

		for(int i=0;i<num_price;i++)name_price[i]=Name_price[i];

		issale=0;

	}

	train& operator=(const train& b){

		id=b.id;name=b.name;catalog=b.catalog;

		num_price=b.num_price;

		for(int i=0;i<num_price;i++)name_price[i]=b.name_price[i];

		num_station=b.num_station;

		for(int i=0;i<num_station;i++)station[i]=b.station[i];

		issale=b.issale;

		return *this;

	}

	void print(){

		id.print();printf(" ");

		name.print();printf(" ");

		catalog.print();printf(" ");

		printf("%d %d ",num_station,num_price);

		

		

		for(int i=0;i<num_price;i++){

			name_price[i].print();

			if(i==num_price-1)printf("\n");

			else printf(" ");			

		}

		for(int i=0;i<num_station;i++)station[i].print();

	}

	int at(String<40> loc){

		for(int i=0;i<num_station;i++)

			if(station[i].name==loc)return i;

		return -1;

	}

};



struct ticket{

	String<40> loc1,loc2;

	String<20> train_id;

	String<12> data;

	String<6> time_start,time_arrive;

	int id;

	int num_price;//票的种类name

	double money[6];

	String<40> name_price[6];

	int number_price[6];//data这一天的各种种类的票的数目

	int num_buy[6];

	

	int at(String<40> ticket_kind){

		for(int i=0;i<num_price;i++)

			if(name_price[i]==ticket_kind)return i;

		return -1;

	}

	ticket(){

		id=num_price=0;

		for(int i=0;i<6;i++)money[i]=0,number_price[i]=0,num_buy[i]=0;

	}

	

	friend bool operator<(const ticket& a,const ticket& b){

		return a.train_id<b.train_id;

	}

	ticket& operator=(const ticket& b){

		loc1=b.loc1;loc2=b.loc2;

		train_id=b.train_id;

		data=b.data;

		time_start=b.time_start;

		time_arrive=b.time_arrive;

		id=b.id;

		num_price=b.num_price;

		for(int i=0;i<num_price;i++)money[i]=b.money[i];

		for(int i=0;i<num_price;i++)name_price[i]=b.name_price[i];

		for(int i=0;i<num_price;i++)number_price[i]=b.number_price[i];

		for(int i=0;i<num_price;i++)num_buy[i]=b.num_buy[i];

		return *this;

	}

	void print(){

		train_id.print();printf(" ");

		

		loc1.print();printf(" ");

		data.print();printf(" ");

		time_start.print();printf(" ");

		

		loc2.print();printf(" ");

		data.print();printf(" ");

		time_arrive.print();printf(" ");

		

		for(int i=0;i<num_price;i++){

			name_price[i].print();printf(" ");

			printf("%d ",number_price[i]);

			printf("¥%.2lf%c",money[i],i==num_price-1?'\n':' ');

		}

	}

	

	void print2(){

		train_id.print();printf(" ");

		

		loc1.print();printf(" ");

		data.print();printf(" ");

		time_start.print();printf(" ");

		

		loc2.print();printf(" ");

		data.print();printf(" ");

		time_arrive.print();printf(" ");

		

		for(int i=0;i<num_price;i++){

			name_price[i].print();printf(" ");

			printf("%d ",num_buy[i]);

			printf("¥%.2lf%c",money[i],i==num_price-1?'\n':' ');

		}

		

		

	}

	void input2(train& now,int s,int t,String<12> Data){

		loc1=now.station[s].name;loc2=now.station[t].name;

		train_id=now.id;

		data=Data;

		time_start=now.station[s].start_time;

		time_arrive=now.station[t].arrive_time;

		num_price=now.num_price;

		for(int i=0;i<num_price;i++)money[i]=0,number_price[i]=2000;

		for(int i=0;i<num_price;i++)name_price[i]=now.name_price[i];

		

		for(int i=s+1;i<=t;i++){

			for(int j=0;j<num_price;j++){

				money[j]+=now.station[i].price[j];

				number_price[j]=std::min(number_price[j],now.station[i].num_remain[Data.trans2()][j]);

			}

		}

		for(int i=0;i<num_price;i++)num_buy[i]=0;

	}

};







Bptree<int,user> USER("user");//用户数据库

/*

车次数据库

*/



Bptree<String<20>,train> TRAIN("train");//根据train_id排序

Bptree<String<40>,String<20> >  Train_Index("train_index");

struct KEY2{

	int id;

	String<12> data;

	String<20> train_id; 

	String<40> loc1,loc2;

	KEY2(){}

	KEY2(int Id,String<12> Data,String<20> Train_id="",String<40> Loc1="",String<40> Loc2=""){

		id=Id;

		data=Data;

		train_id=Train_id;

		loc1=Loc1;

		loc2=Loc2;

	}

	

	friend bool operator==(KEY2 a,KEY2 b){

		if(b.train_id==""||a.train_id=="")return a.id==b.id&&a.data==b.data;

		else return a.id==b.id&&a.data==b.data&&a.train_id==b.train_id&&a.loc1==b.loc1&&a.loc2==b.loc2;

	}

	friend bool operator!=(KEY2 a,KEY2 b){

		return !(a==b);

	}

	friend bool operator<(const KEY2& a,const KEY2& b){

		if(a.id!=b.id)

			return a.id<b.id;

		if(a.data!=b.data)

			return a.data<b.data;

		if(a.train_id!=b.train_id)

			return a.train_id<b.train_id;

		if(a.loc1!=b.loc1)

			return a.loc1<b.loc1;

		if(a.loc2!=b.loc2)

			return a.loc2<b.loc2;

		return 0;

	}

};

Bptree<KEY2,ticket> TICKET("ticket");

/*

功能：检查该id用户是否存在

返回值：存在返回1，不存在返回0

*/

bool check(int id){

	if(id<2019||id>USER.size()+2017)return 0;

	return 1;

}



/*

功能：注册。

返回值：1。

问题：未加入注册失败功能。

*/

void Register(String<40> name,String<20> password,String<20> email,String<20> phone){

	user now(name,password,email,phone);

	now.id=USER.size()+2018;

	if(now.id==2019)now.privilege=2;

	USER.insert(std::pair<int,user>(now.id,now));

	printf("%d\n",now.id);

}







/*

功能：登录

返回值：成功1，失败0

*/

void Login(int id,String<20> Password){

	if(!check(id)){

		printf("0\n");

		return ;

	}

	user now=(*(USER.find(id))).second;

	if(now.password==Password)printf("1\n");

	else printf("0\n");

}





/*

功能：输出用户信息

返回值：无

*/

void query_profile(int id){

	if(!check(id)){

		printf("0\n");

		return ;

	}

	user now=(*(USER.find(id))).second;

	now.print();

}



/*

功能： 修改用户信息

返回值：成功1，失败0

*/

int modify_profile(int id,String<40> Name,String<20> Password,

	String<20> Email,String<20> Phone){

	if(!check(id))return 0;

	user now=(*(USER.find(id))).second;

	now.change(Name,Password,Email,Phone);

	USER.erase(id);USER.insert(std::pair<int,user>(id,now));

	return 1;

}





/*

功能：ID1用户改变id2用户优先级

返回值:成功为1，失败为0



*/

int modify_privilege(int id1,int id2,int pri){

	if(!check(id1)||!check(id2))return 0;

	user now1=(*(USER.find(id1))).second,now2=(*(USER.find(id2))).second;

	if(now1.privilege==1)return 0;

	if(now2.privilege==2&&pri==1)return 0;

	now2.privilege=pri;

	USER.erase(id2);USER.insert(std::pair<int,user>(id2,now2));

	return 1;

}





//--------------------------------------------------------------------------------

//以下为车次相关内容

//--------------------------------------------------------------------------------

/*

所需要的功能：

int find(char*):查询当前火车ID是否存在，存在返回0，否则返回1.

train get(char*) :获得ID所指向的train

Delete(char*) 删除ID所指向的train

*/















/*
查询火车ID是否存在，bu存在返回0，存在返回1
*/

bool check_trainID(String<20> ID){

	if(TRAIN.count(ID))return 1;

	else return 0;

}







/*

功能：添加车次

返回值，成功 1，失败为0；



注意：必须配套station

*/

int add_train(String<20> Id,String<40> Name,String<10> Catalog,int Num_station,

int num_price,String<40> name_price[],Station station[]){

	if(check_trainID(Id))return 0;

	train now(Id,Name,Catalog,Num_station,num_price,name_price,station);

	TRAIN.insert(std::pair<String<20>,train>(Id,now));

	//now.print();

	return 1;

}

/*

公开车次

返回:成功为1，失败为0。

问题：如何处理？

*/

int sale_train(String<20> ID){

	if(!check_trainID(ID))return 0;

	train now=(*(TRAIN.find(ID))).second;

	if(now.issale)return 0;

	now.issale=1;

	TRAIN.erase(ID);TRAIN.insert(std::pair<String<20>,train>(ID,now));

	

	for(int i=0;i<now.num_station;i++)

		Train_Index.insert(std::pair<String<40>,String<20> >(now.station[i].name,ID));

	

	return 1;

}





/*

查询车次

无返回值

*/

void query_train(String<20> ID){

	if(!check_trainID(ID)){

		printf("0\n");

		return;

	}

	train now=(*(TRAIN.find(ID))).second;

	if(!now.issale){

		printf("0\n");

		return;

	}

	now.print();

}



/*

删除车次

成功为1，失败为0

*/



int delete_train(String<20> ID){

	if(!check_trainID(ID))return 0;

	train now=(*(TRAIN.find(ID))).second;

	if(now.issale)return 0;

	TRAIN.erase(ID);

	return 1;

}



/*

修改车次

成功为1，失败为0

*/





int modify_train(String<20> Id,String<40> Name,String<10> Catalog,int Num_station,

int num_price,String<40> name_price[],Station station[]){

	if(!check_trainID(Id))return 0;

	train now=(*(TRAIN.find(Id))).second;

	if(now.issale)return 0;

	train now2(Id,Name,Catalog,Num_station,num_price,name_price,station);

	TRAIN.erase(Id);

	TRAIN.insert(std::pair<String<20>,train>(Id,now2));

	return 1;

}



//--------------------------------------------------------------------------------

//以下为管理相关内容

//--------------------------------------------------------------------------------

/*

	需要的命令：

	clean() 删库跑路

*/

//你也懂

int clean(){

	USER.clear();

	TRAIN.clear();

	Train_Index.clear();

	TICKET.clear();

	return 1;

}

//你同样懂

void Exit(){

	printf("BYE\n");

	exit(0);

} 

//--------------------------------------------------------------------------------

//以下为车票相关内容

//--------------------------------------------------------------------------------

/*

需要接口：

 find(catalog,loc1) 查找所有catalog类型的过loc1的车 返回<train*,num> 

		注意：要求先按照loc排序，再按照id排序





需要vector





*/





/*

查询车票loc1->loc2 data Catalog

方法：先按照Catalog分类，再找到所有经过loc1的车，对这些车进行遍历找到所有过loc2的车

返回：无返回值

*/





bool query_ticket(String<40> Loc1,String<40> Loc2,String<12> Data,String<10> Catalog){

	auto it=Train_Index.find(Loc1);

	int sum=0;

	ticket t[100];

	if(Loc1==Loc2)return 0;

	if(Data<(String<12>("2019-06-01"))||Data>(String<12>("2019-06-30")))return 0;

	

	while((*it).first==Loc1&&it!=Train_Index.end()){

		train now=(*(TRAIN.find((*it).second))).second;it++;

		if(!now.catalog.beibaohan(Catalog))continue;

		int k1=now.at(Loc1),k2=now.at(Loc2);

		if(k1>=0&&k1<k2)

			t[sum++].input2(now,k1,k2,Data);

	}

	

	printf("%d\n",sum);

	for(int i=0;i<sum;i++)

		t[i].print();

	return 1;

}



void check(train now1,train now2,int k1,int k2,String<12> data,

ticket& ans1,ticket& ans2,bool& flag,int& ans

){

	if((k1!=0&&now1.station[k1].arrive_time.trans()>now2.station[k2].start_time.trans())||

	(k1==0&&now1.station[k1].start_time.trans()>now2.station[k2].start_time.trans()))

		return ;

	//now1.print(),now2.print();

	for(int i=k1+1;i<now1.num_station;i++){

		if(now1.station[i].arrive_time.trans()>now2.station[k2].start_time.trans())break;

		for(int j=k2-1;j>=0;j--){

			if(now1.station[i].arrive_time.trans()>now2.station[j].start_time.trans())break;

			if(now1.station[i].name==now2.station[j].name){

				int iterval=now2.station[k2].arrive_time.trans()-

						now1.station[k1].start_time.trans();

				if(iterval<ans){

					//printf("FIND IT!\n");

					ans=iterval;

					ans1.input2(now1,k1,i,data);

					ans2.input2(now2,j,k2,data);

					flag=1;

				}

			}

		}

	}

	return ;

}



bool query_transfer(String<40> loc1,String<40> loc2,String<12> data,String<10> catalog)

{

	

	

	if(loc1==loc2)return 0;

	if(data<(String<12>("2019-06-01"))||data>(String<12>("2019-06-30")))return 0;

	

	

	auto it1=Train_Index.find(loc1);

	

	int ans=inf;ticket ans1,ans2;bool flag=0;

	while((*it1).first==loc1&&it1!=Train_Index.end()){

		

		train now1=(*(TRAIN.find((*it1).second))).second;it1++;

		

		//printf("A\n");

		//now1.print();

		

		if(!now1.catalog.beibaohan(catalog))continue;

		int k1=now1.at(loc1);

		

		auto it2=Train_Index.find(loc2);

		while((*it2).first==loc2&&it2!=Train_Index.end()){

			

			train now2=(*(TRAIN.find((*it2).second))).second;it2++;

			

			

			

			if(!now2.catalog.beibaohan(catalog))continue;

			int k2=now2.at(loc2);

			//printf("B\n");

			//now2.print();

			check(now1,now2,k1,k2,data,ans1,ans2,flag,ans);

		}

	}

	if(flag){

		ans1.print();

		ans2.print();

	}

	else puts("-1");

	return 1;

}

bool buy_ticket(int id,int num,String<20> train_id,String<40> loc1,

String<40> loc2,String<12> data,String<40> ticket_kind)

{

	if(loc1==loc2)return 0;

	if(data<(String<12>("2019-06-01"))||data>(String<12>("2019-06-30")))return 0;

	if(!TRAIN.count(train_id))return 0;

	

	

	train now=((*(TRAIN.find(train_id)))).second;

	

	int k1=now.at(loc1),k2=now.at(loc2);

	if(k1==-1||k2==-1||k1>k2)return 0;

	int day=data.trans2();

	

	KEY2 key(id,data,now.id,loc1,loc2);

	ticket t;

	

	Bptree<KEY2,ticket>::iterator it;


		it=TICKET.find(key);

	if(it!=TICKET.end()&&(*it).first==key){

		t=(*it).second;

		TICKET.erase(key);

	}

	else t.input2(now,k1,k2,data);

	for(int i=0;i<t.num_price;i++)t.number_price[i]=0;

	t.id=id;

	int k=t.at(ticket_kind);

	if(k==-1)return 0;

	

	t.num_buy[k]+=num;

	

	

	

	for(int i=k1+1;i<=k2;i++){

		if(now.station[i].num_remain[day][k]>=num)

			now.station[i].num_remain[day][k]-=num;

		else return 0;

	}

	printf("1\n");

	

	

	TICKET.insert(std::pair<KEY2,ticket>(key,t));

	TRAIN.erase(train_id);TRAIN.insert(std::pair<String<20>,train>(train_id,now));

	return 1;

}



bool query_order(int id,String<12> data,String<10> catalog){

	

	

	if(data<(String<12>("2019-06-01"))||data>(String<12>("2019-06-30")))return 0;

	if(!check(id))return 0;

	

	

	auto it=TICKET.find(KEY2(id,data));

	int sum=0;

	while((*it).first==KEY2(id,data)&&it!=TICKET.end()){

		train now=(*(TRAIN.find(((*it).second.train_id)))).second;

		if(now.catalog.beibaohan(catalog))

			sum++;

		it++;

	}

	printf("%d\n",sum);

	it=TICKET.find(KEY2(id,data));

	while((*it).first==KEY2(id,data)&&it!=TICKET.end()){

		train now=(*(TRAIN.find(((*it).second.train_id)))).second;

		if(now.catalog.beibaohan(catalog))

			(*it).second.print2();

		it++;

	}

	return 1;

}



bool refund_ticket(int id,int num,String<20> train_id,String<40> loc1,

String<40> loc2,String<12> data,String<40> ticket_kind)

{

	if(!TRAIN.count(train_id))return 0;

	train now=(*(TRAIN.find(train_id))).second;

	KEY2 key(id,data,train_id,loc1,loc2);

	

	auto it=TICKET.find(key);

	while((*it).first==key&&it!=TICKET.end()){

		ticket t=(*it).second;

		

		

		if(t.train_id==train_id&&t.id==id&&

		t.loc1==loc1&&t.loc2==loc2&&t.data==data&&

		t.at(ticket_kind)!=-1){

			int k=t.at(ticket_kind);

			int day=data.trans2();

			if(t.num_buy[k]<num)return 0;

			else t.num_buy[k]-=num;

			

			int k1=now.at(loc1),k2=now.at(loc2);

			

			for(int i=k1+1;i<=k2;i++)

				now.station[i].num_remain[day][k]+=num;

			

			TRAIN.erase(t.train_id);

			TRAIN.insert(std::pair<String<20>,train>(train_id,now));

			

			TICKET.erase(key);

			bool flag=0;

			for(int i=0;i<t.num_price;i++)

				if(t.num_buy[i])flag=1;

			if(flag)TICKET.insert(std::pair<KEY2,ticket>(key,t));

			printf("%d\n",1);

			return 1;

		}

		it++;

	}

	return 0;

}

















String<20> sregister("register"),slogin("login"),squery_profile("query_profile"),

smodify_profile("modify_profile"),smodify_privilege("modify_privilege"),

squery_ticket("query_ticket"),squery_transfer("query_transfer"),sbuy_ticket("buy_ticket"),

squery_order("query_order"),srefund_ticket("refund_ticket"),

sadd_train("add_train"),ssale_train("sale_train"),squery_train("query_train"),

sdelete_train("delete_train"),smodify_train("modify_train"),sclean("clean"),

sexit("exit");







int main(){

	//freopen("out","w",stdout);

	char op[45];

	if(TICKET.size()==0){
		ticket t;
		KEY2 key;
		TICKET.insert(std::pair<KEY2,ticket>(key,t));\
	}
	if(Train_Index.size()==0){
		String<40> key;
		String<20> train_id;
		Train_Index.insert(std::pair<String<40>,String<20> >(key,train_id));
	}
	if(TRAIN.size()==0){
		train val;
		String<20> train_id;
		TRAIN.insert(std::pair<String<20>,train >(train_id,val));
	}
	if(USER.size()==0){
		int k=0;
		user val;
		USER.insert(std::pair<int,user >(k,val));
	}
	while(true){

		scanf("%s",op);

		String<20> p(op);

		

		if(p==sregister){

			scanf("%s",op);String<40> name(op);

			scanf("%s",op);String<20> password(op);

			scanf("%s",op);String<20> email(op);

			scanf("%s",op);String<20> phone(op);

			Register(name,password,email,phone);

			continue;

		}

		if(p==slogin){

			int id;scanf("%d",&id);

			scanf("%s",op);String<20> password(op);

			Login(id,password);

			continue;

		}

		if(p==squery_profile){

			int id;scanf("%d",&id);

			query_profile(id);

			continue;

		}

		if(p==smodify_profile){

			int id;scanf("%d",&id);

			scanf("%s",op);String<40> name(op);

			scanf("%s",op);String<20> password(op);

			scanf("%s",op);String<20> email(op);

			scanf("%s",op);String<20> phone(op);

			printf("%d\n",modify_profile(id,name,password,email,phone));

			continue;

		}

		if(p==smodify_privilege){

			int id1;scanf("%d",&id1);

			int id2;scanf("%d",&id2);

			int pri;scanf("%d",&pri);

			printf("%d\n",modify_privilege(id1,id2,pri));

			continue;

		}

		

		

		if(p==squery_ticket){

			scanf("%s",op);String<40> loc1(op);

			scanf("%s",op);String<40> loc2(op);

			scanf("%s",op);String<12> data(op);

			scanf("%s",op);String<10> catalog(op);

			if(!query_ticket(loc1,loc2,data,catalog))printf("0\n");

			

			continue;

		}

		if(p==squery_transfer){

			scanf("%s",op);String<40> loc1(op);

			scanf("%s",op);String<40> loc2(op);

			scanf("%s",op);String<12> data(op);

			scanf("%s",op);String<10> catalog(op);

			if(!query_transfer(loc1,loc2,data,catalog))printf("0\n");

			

			continue;

		}

		if(p==sbuy_ticket){

			int id;scanf("%d",&id);

			int num;scanf("%d",&num);

			

			scanf("%s",op);String<20> train_id(op);

			scanf("%s",op);String<40> loc1(op);

			scanf("%s",op);String<40> loc2(op);

			scanf("%s",op);String<12> data(op);

			scanf("%s",op);String<40> ticket_kind(op);

			

			if(!buy_ticket(id,num,train_id,loc1,loc2,data,ticket_kind))printf("0\n");

			

			continue;

		}

		if(p==squery_order){

			int id;scanf("%d",&id);

			scanf("%s",op);String<12> data(op);

			scanf("%s",op);String<10> catalog(op);

			

			if(!query_order(id,data,catalog))printf("0\n");

			continue;

		}

		if(p==srefund_ticket){

			

			int id;scanf("%d",&id);

			int num;scanf("%d",&num);

			

			scanf("%s",op);String<20> train_id(op);

			scanf("%s",op);String<40> loc1(op);

			scanf("%s",op);String<40> loc2(op);

			scanf("%s",op);String<12> data(op);

			scanf("%s",op);String<40> ticket_kind(op);

			

			if(!refund_ticket(id,num,train_id,loc1,loc2,data,ticket_kind))printf("0\n");

			

			continue;

		}

		if(p==sadd_train){
			scanf("%s",op);String<20> train_id(op);
			scanf("%s",op);String<40> name(op);
			scanf("%s",op);String<10> catalog(op);
			int num_station;scanf("%d",&num_station);
			int num_price;scanf("%d",&num_price);
			String<40> name_price[6];
			for(int i=0;i<num_price;i++){
				scanf("%s",&op);
				name_price[i]=op;
			}
			Station station[60];
			for(int i=0;i<num_station;i++)station[i].input(num_price,name_price);
			//station[0].arrive_time=station[0].start_time;
			printf("%d\n",add_train(train_id,name,catalog,num_station,num_price,name_price,station));
			continue;

		}

		if(p==ssale_train){

			scanf("%s",op);String<20> train_id(op);

			

			printf("%d\n",sale_train(train_id));

			

			continue;

		}

		if(p==squery_train){

			

			scanf("%s",op);String<20> train_id(op);

			

			query_train(train_id);

			

			continue;

			

		}

		if(p==sdelete_train){

			

			scanf("%s",op);String<20> train_id(op);

			

			printf("%d\n",delete_train(train_id));

			

			continue;

			

			

		}

		if(p==smodify_train){

			

			scanf("%s",op);String<20> train_id(op);

			scanf("%s",op);String<40> name(op);

			scanf("%s",op);String<10> catalog(op);

			int num_station;scanf("%d",&num_station);

			int num_price;scanf("%d",&num_price);

			String<40> name_price[6];

			for(int i=0;i<num_price;i++){

				scanf("%s",&op);

				name_price[i]=op;

			}

			Station station[60];

			for(int i=0;i<num_station;i++){

				station[i].input(num_price,name_price);

			}//station[0].arrive_time=station[0].start_time;

			printf("%d\n",modify_train(train_id,name,catalog,num_station,num_price,name_price,station));

			

			continue;

			

		}

		

		if(p==sclean){

			printf("%d\n",clean());

			continue;

		}

		if(p==sexit){
			cout<<sjtu::ans<<endl;
			Exit();

			continue;

		}

		printf("未识别\n");

		

	}

	

	

}



