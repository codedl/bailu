//============================================================================
// Name        : ch16.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstring>
#include <string>
#include <memory>

using namespace std;
void stringSize();
void smrtptrs();
class Report
{
private:
	std::string str;
public:
	Report(const string str) : str(str)
	{cout << "object created!\n";}
	~Report(){cout << "object deleted!\n";}
	void comment() const {std::cout << str << "\n";}
};
int main() {
	smrtptrs();
	return 0;
}
void stringSize()
{
	string empty;
	string small = "bit";
	string larger = "Elephants are a girl's best friends";
	cout << "size:\n";
	cout << "\tempty: " << empty.size() << endl;
	cout << "\tsamll: " << small.size() << endl;
	cout << "\tlarger: " << larger.size() << endl;
	cout << "capacities:\n";
	cout << "\tempty: " << empty.capacity() << endl;
	cout << "\tsamll: " << small.capacity() << endl;
	cout << "\tlarger: " << larger.capacity() << endl;
	empty.reserve(50);
	cout << "capacities after empty.reserve(50): "<< empty.capacity() << endl;
}

void smrtptrs()
{
	{
		std::auto_ptr<Report> ps (new Report("using auto_ptr"));
		ps->comment();
	}
	{
		std::shared_ptr<Report> ps (new Report("using shared_ptr"));
		ps->comment();

	}
	{
		std::unique_ptr<Report> ps (new Report("using unique_ptr"));
		ps->comment();
	}
}
