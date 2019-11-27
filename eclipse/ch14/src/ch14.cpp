//============================================================================
// Name        : ch14.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "studentc.h"
#include "worker0.h"
#include "arraytp.h"
#include "stacktp.h"
using namespace std;
const int pupils = 3;
const int quizzes = 5;
void set(Student &s, int n);
void stuTest();
void workTest();
void arrayTest();
void templateAsParam();
int main() {
	templateAsParam();
	return 0;
}
void set(Student &stu, int n)
{
	cout << "enter name: ";
	getline(cin, stu);
	cout << "enter " << n << " quiz scores:\n";
	for(int i=0; i<n; i++)
	{
		cin>>stu[i];
	}
	while(cin.get() != '\n')
		continue;
}
void stuTest()
{
	Student ada[pupils] =
	{Student(quizzes),Student(quizzes),Student(quizzes)};
	int i;
	for(i=0; i<pupils; i++)
		set(ada[i], quizzes);
	cout << "\nStudent List:\n";
	for(i=0; i<pupils; i++)
		cout << ada[i].Name() << endl;
	cout << "\nResults: ";
	for(i=0; i<pupils; ++i)
	{
		cout << endl << ada[i];
		cout << "average: " << ada[i].Average() << endl;
	}
	cout << "Done.\n";
}

void workTest()
{
	Waiter bob("Bob Apple", 314L, 5);
	Singer bev("Beverly Hills", 522L, 3);
	Waiter w_temp;
	Singer s_temp;
	Worker *pw[4] = {&bob, &bev, &w_temp, &s_temp};

	int i=0;
	for(i=2; i<4; i++)
		pw[i] -> Set();
	for(i=0; i<4; i++)
	{
		pw[i]->Show();
		cout << "\n";
	}
}
void arrayTest()
{
	ArrayTp<int, 10> sum;
	ArrayTp<double, 10>aves;
	ArrayTp<ArrayTp<int,5>, 10> twodee;

	int i=0,j=0;
	for(i=0; i<10; i++)
	{
		sum[i] = 0;
		for(j=0; j<5; j++)
		{
			twodee[i][j] = (i+1) * (j+1);
			sum[i] += twodee[i][j];
		}
		aves[i] = (double)sum[i] / 10;
	}

	for(i=0; i<10; i++)
	{
		for(j=0; j<5; j++)
		{
			cout.width(2);
			cout << twodee[i][j] << "  ";
		}
		cout.width(3);
		cout << "sum: " << sum[i] << ", average = "<<aves[i] << endl;
	}
	cout << "Done.\n";
}

void templateAsParam()
{
	Crab<Stack> nebula;
	int ni;
	double nb;
	cout << "enter int double pairs:\n";
	while(cin >> ni >> nb && ni > 0 && nb > 0)
	{
		if(!nebula.push(ni,nb))
			break;
	}
	while(nebula.pop(ni, nb))
		cout << ni << ", " << nb << endl;
	cout << "Done.\n";
}
