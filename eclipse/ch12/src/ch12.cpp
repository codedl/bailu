//============================================================================
// Name        : ch12.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <new>
#include "stringbad.h"
using namespace std;
const int BUF = 512;
class JusTesting
{
private:
	string words;
	int number;
public:
	JusTesting(const string &s = "Just Testing", int n=0)
	{words = s; number = n; cout << words << " constructed\n";}
	~JusTesting(){cout << words << "destroyed\n";}
	void show()const{cout << words << ", " << number << endl;}
};
void callme1(StringBad &);
void callme2(StringBad );
void demo1();
void newtest();
int main() {
//	demo1();
	newtest();
	return 0;
}
void callme1(StringBad &rsb)
{
	cout << "string passed by reference:\n";
	cout << "	\"" << rsb << "\"\n";
}

void callme2(StringBad sb)
{
	cout << "string passed by value:\n";
	cout << "	\"" << sb << "\"\n";
}
void demo1()
{
	StringBad headline1("Celery Stalks at Midnight");
	StringBad headline2("Lettuce Prey");
	StringBad sports("Spinach Leaves Bowl for Dollars");
	cout << "headline1:" << headline1 << endl;
	cout << "headline2:" << headline2 << endl;
	cout << "sport:" << sports << endl;
	callme1(headline1);
	cout << "headline1:" << headline1 << endl;
	callme2(headline2);
	cout << "headline2:" << headline2 << endl;
	cout << "init one object to another:\n";
	StringBad sailor = sports;
	cout << "sailor: " << sailor << endl;
	cout << "assign one object to another:\n";
	StringBad knot;
	knot = headline1;
	cout << "knot: " << knot << endl;
	cout << "Exitint the block.\n";
}

void newtest()
{
	char *buffer = new char[BUF];
	JusTesting *pc1, *pc2;
	pc1 = new (buffer) JusTesting;
	pc2 = new JusTesting("heap",20);
	cout << "Memory block address:\n" << "buffer: "
			<< (void *)buffer << "	heap:" << pc2 << endl;
	cout << "Memory contents:\n";
	cout << pc1 << ": ";
	pc1->show();
	cout << pc2 << ": ";
	pc2->show();

	JusTesting *pc3, *pc4;
	pc3 = new (buffer + sizeof (JusTesting)) JusTesting("Bad Idea",6);
	pc4 = new JusTesting("heap2",10);

	cout << "Memory contents:\n";
	cout << pc3 << ": ";
	pc3->show();
	cout << pc4 << ": ";
	pc4->show();

	delete pc2;
	delete pc4;

	pc3->~JusTesting();
	pc1->~JusTesting();
	delete[]buffer;
	cout << "done\n";
}
