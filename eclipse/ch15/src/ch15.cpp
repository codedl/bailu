//============================================================================
// Name        : ch15.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <typeinfo>

#include "queuetp.h"
using namespace std;
void queueTest();
double hmean(double a, double b);
void hmeanTest();
void exceptionTest();
class Grand
{
private:
	int hold;
public:
	Grand(int h=0):hold(h){}
	virtual void Speak()const{cout << "i am a grand class\n";}
	virtual int Value()const{return hold;}
};
class Superb : public Grand
{
public:
	Superb(int h=0):Grand(h){}
	void Speak()const{cout << "i am a superb class \n";}
	virtual void Say() const
	{cout << "i hold the superb value of " << Value() << "\n";	}
};
class Magnificent : public Superb
{
private:
	char ch;
public:
	Magnificent(int h=0, char c='A'):Superb(h),ch(c){}
	void Speak()const{cout << "i am a magnificent class\n";}
	void Say()const{cout << "i hold the char " << ch <<
						"  and the integer " << Value() << " !\n";}
};
Grand * GetOne();
void GrandTest();
int main() {
	GrandTest();
	return 0;
}
void queueTest()
{
	QueueTp<string> cs(5);
	string temp;
	while(!cs.isFull())
	{
		cout << "enter string:\n";
		getline(cin,temp);
		cs.enqueue(temp);
	}
	while(!cs.isEmpty())
	{
		cs.dequeue(temp);
		cout << "your enter: " << temp << endl;
//		abort();
	}
}

double hmean(double a, double b)
{
	if(a == -b)
		throw "a == -b";
	else
		return 2.0 * a * b/(a+b);
}
void hmeanTest()
{
	double x, y;
	while(cin >> x >> y)
	{
		try{
			cout << "result : " << hmean(x,y);
		}catch (const char *s)
		{
			cout << s << endl;
			continue;
		}

	}
	cout << "Done.\n";
}

void exceptionTest()
{
	struct big {
		double buf[20000];
	};
	try{
		big *p = new big[10000];
	}catch(bad_alloc &e)
	{
		cout << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}
Grand * GetOne()
{
	Grand * p = NULL;
	switch(rand() % 3)
	{
	case 0:
		p = new Grand(rand() % 100);
		break;
	case 1:
		p = new Superb(rand() % 100);
		break;
	case 2:
		p = new Magnificent(rand()%100,'A' + rand()%26);
		break;

	}
	return p;
}

void GrandTest()
{

	srand(time(0));
	Grand *pg;
	Superb *ps;
	for(int i=0; i<5; i++)
	{
		pg = GetOne();
		cout << "Now process type: " << typeid(*pg).name() << ".\n";
		pg->Speak();
		if(ps =dynamic_cast<Superb*> (pg))
			ps->Say();
		if(typeid(Magnificent) == typeid(*pg))
			cout << "yes, you are Magnificent\n";
	}
}
