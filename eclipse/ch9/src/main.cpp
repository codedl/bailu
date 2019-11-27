//============================================================================
// Name        : ch9.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <new>
#include "coordin.h"
#include "namespace.h"
using namespace std;

int tom = 3;
int dick = 30;
static int harry = 300;
const int ArSize = 10;
const int BUF = 512;
const int N = 5;
char buffer[BUF];
void strcount(const char *str);
void remote_access();
void new_test();
void other(void);
void another(void);
int main() {
	/*rect rplace;
	polar pplace;
	cout << "enter the x and y values:";
	while(cin >> rplace.x >> rplace.y){
		pplace = rect_to_polar(rplace);
		show_polar(pplace);
		cout<<"Next two number(q to quit)";
	}*/
#if 0
	int texas = 31;
	int year = 2019;
	cout << "in main:texas" << texas << "&texas:" << &texas << endl;
	cout << "in main:year" << year << "&year:" << &year << endl;
	oil(texas);
	cout << "in main:texas" << texas << "&texas:" << &texas << endl;
	cout << "in main:year" << year << "&year:" << &year << endl;

	cout << "\nin main:\n" << "&tom:" << &tom << "&dick:" << &dick << "&harry:" << &harry <<endl;
#endif
	//remote_access();
	//new_test();
	using debts::Debt;
	using debts::showDebt;
	Debt golf = {{"Benny","Goatsniff"},120.0};
	showDebt(golf);
	other();
	another();
	return 0;
}

void strcount(const char* str){
	static int total = 0;
	int count = 0;
	cout << "\"" << str << "\" contains";
	while(*str++)
		count ++;
	total += count;
	cout << count << "chars \n";
	cout << total << "chars total\n";
}
void foo(){
char input [ArSize];
	char next;
	cout << "\nenter a line:\n";
	cin.get(input,ArSize);

	while(cin){
		cin.get(next);
		while(next != '\n'){
			cin.get(next);
			cout << "next :" << next <<endl;
		}
		strcount(input);
		cout << "enter next line(empty line to quit):\n";
		cin.get(input,ArSize);
	}
	new_test();
	cout<<"Bye\n";
}
void new_test(){
	double *pd1, *pd2;
	int i;
	cout << "calling new :\n";
	pd1 = new double[N];
	pd2 = new (buffer)double[N];
	for(i=0; i<N; i++)
		pd2[i] = pd1[i] = 1000 + 20.0*i;
	cout << "memory address:\n"<<"heap: " << pd1
			<< " static: " << (void*) buffer << endl;
	cout << "memory contents:\n";
	for(i=0; i<N; i++)
	{
		cout << pd1[i] << " at " << &pd1[i] << endl;
		cout << pd2[i] << " at " << &pd2[i] << endl;
	}

	cout << "\ncalling new a second time:\n";
	double *pd3, *pd4;
	pd3 = new double[N];
	pd4 = new (buffer)double[N];
	for(i=0; i<N; i++)
		pd4[i] = pd3[i] = 1000 +40.0*i;
	cout << "memory content:\n";
	for(i=0; i<N; i++)
	{
		cout << pd3[i] << " at " << &pd3[i] << endl;
		cout << pd4[i] << " at " << &pd4[i] << endl;
	}
	cout << "\ncalling new a third time:\n";
	delete [] pd1;//delete pd1;
	pd1 = new double[N];
	pd2 = new (buffer +N * sizeof (double)) double[N];
	for(i=0; i<N; i++)
		pd2[i] = pd1[i] = 1000 + 60.0 * i;
	cout<< "memory content:\n";
	for(i=0; i<N; i++){
		cout << pd1[i] << " at " << &pd1[i] << endl;
		cout << pd2[i] << " at " << &pd2[i] << endl;
	}
	delete []pd1;
	delete []pd3;
}

void other(void){
	using std::cout;
	using std::endl;
	using namespace debts;
	Person dg = {"Doodles", "Glister"};
	showPerson(dg);
	cout << endl;
	Debt zippy[3];
	int i;
	for(i=0; i<3; i++)
		getDebt(zippy[i]);
	for(i=0; i<3; i++)
		showDebt(zippy[i]);
	cout << "total debt: $" << sumDebts(zippy,3) << endl;
}

void another(void){
	using pers::Person;
	Person collector = {"Milo", "Rightshift"};
	pers::showPerson(collector);
	std::cout << std::endl;
}
