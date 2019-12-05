//============================================================================
// Name        : ch15.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
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
	virtual void Speak()const{cout << "i am a grand class";}
	virtual int Value()const{return hold;}
};
class Superb : public Grand
{
public:
	Superb(int h=0):Grand(h){}
	void Speak()const{cout << "i am a superb class";}
	virtual void Say() const
	{cout << "i hold the superb value of " << Value();	}
};
class Magnificent : private Superb
{
private:
	char ch;
public:
	Magnificent(int h=0, char c='A'):Superb(h),ch(c){}
};

int main() {
	exceptionTest();
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
