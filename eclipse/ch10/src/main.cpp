//============================================================================
// Name        : ch10.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "stock00.h"
#include "stack.h"
using namespace std;
const int STKS = 4;
void Stock00_test(void);
void Stock01_test(void);
void Stock02_test(void);
void stack_test(void);
int main() {
	Stock02_test();
	return 0;
}

void Stock00_test(void){
	Stock fluffy_the_cat;
	fluffy_the_cat.acquire("NanoSmart", 20, 12.5);
	fluffy_the_cat.show();
	fluffy_the_cat.buy(15, 18.125);
	fluffy_the_cat.show();
	fluffy_the_cat.sell(400, 20.00);
}

void Stock01_test(void){
	using std::cout;
	cout << "using constructor to create new objects\n";
	Stock stock1("NanoSmart", 12, 20.0);//创建一个对象
	stock1.show();
	Stock stock2 = Stock("Boffo", 2, 2.0);//可能创建一个临时对象将值赋给stock2
	stock2.show();

	cout << "assigning stock1 to stock2:\n";
	stock2 = stock1;
	stock1.show();
	stock2.show();

	cout << "using a constructor to reset an object\n";
	stock1 = Stock("Niffy Foods", 10, 50.0);//一定创建一个临时对象将值赋给stock2,然后注销
	cout << "Revised stock1:\n";
	stock1.show();
	cout << "Done\n";
}
void Stock02_test(void){
	Stock stocks[STKS] ={
			Stock("NanoSmart",12,20.0),
			Stock("Boffo Objects",200,2.0),
			Stock("Monolithic",130,3.25),
			Stock("Fleep",60,6.5)
	};

	std::cout << "Stock holding:\n";
	int st;
	for(st = 0; st<STKS; st++)
		stocks[st].show();
	const Stock *top = &stocks[0];
	for(st=1; st<STKS; st++)
		top = &top->topval(stocks[st]);
	std::cout << "\nMost valuable holding:\n";
	top->show();

}

