//============================================================================
// Name        : CH8.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
void quote(int &x);
double cube(double x);
double refcube(const double &x);
void file_it(ostream &os, double fo, const double fe[], int n);
const int limit = 5;
int main() {
	int rats=10;
	int &prats = rats;
	cout << "rats:" << rats;
	cout << "prats:" << prats << endl;
	rats ++;
	cout << "rats:" << rats;
	cout << "prats:" << prats << endl;

	cout << "&rats:" << &rats;
	cout << "&prats:" << &prats << endl;
	quote(rats);
	cout << "quote(rats):" << rats << endl;

	double x = 3.0;
	cout << cube(x);
	cout << " = cube of " << x << endl;
	cout << refcube(x);
	cout << " = cube of " << x << endl;

	ofstream fout;
	const char * fn = "ep-data.txt";
	fout.open(fn);
	if(!fout.is_open()){
		cout << "cant open " << fn << ".bye . \n";
		exit(EXIT_FAILURE);
	}
	double object;
	cout << "enter the focal length of your telescope object in mm";
	cin >> object;
	double eps[limit];
	cout << "enter the focal lengths, in mm of" << limit << "eyepiece\n";
	for(int i=0; i<limit; i++){
		cout << "eyepiece #" << i+1 << ":";
		cin >> eps[i];
	}
	file_it(fout,object,eps,limit);
	file_it(cout,object,eps,limit);
	cout << "done\n";
 	return 0;
}
void quote(int &x){
	x += 10;
}
double cube(double x){
	x *= x * x;
	return x;
}
double refcube(const double &x){
	//x  *= x * x;
	return x * x * x;
}

void file_it(ostream &os, double fo, const double fe[], int n){
	ios_base::fmtflags init;
	init = os.setf(ios_base::fixed);
	os.precision(0);
	os << "focal length of objective:" << fo << "mm\n";
	os.setf(ios::showpoint);
	os.precision(1);
	os.width(12);
	os << "f.l. eyeprice";
	os.width(15);
	os << "magnification" << endl;
	for(int i=0; i<n; i++){
		os.width(12);
		os << fe[i];
		os.width(15);
		os << int(fo/fe[i] + 0.5) << endl;
	}
	os.setf(init);
}

