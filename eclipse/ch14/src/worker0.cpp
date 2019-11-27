/*
 * worker0.cpp
 *
 *  Created on: 2019Äê11ÔÂ25ÈÕ
 *      Author: dl
 */

#include <iostream>
#include "worker0.h"
using std::cout;
using std::cin;
using std::endl;
Worker::~Worker(){}

void Worker::Set()
{
	cout << "enter fullname:\n";
	getline(cin,fullname);
	cout << "enter worker id:\n";
	cin >> id;
	while(cin.get() != '\n')
		continue;
}
void Worker::Show()const
{
	cout << "Name: " << fullname << endl;
	cout << "Employee Id: "<<id<<endl;
}

void Waiter::Set()
{
	Worker::Set();
	cout << "Enter waiter's panache rating: ";
	cin >> panache;
	while(cin.get() != '\n')
		continue;
}
void Waiter::Show() const
{
	cout << "Cateory waiter:\n";
	Worker::Show();
}

char *Singer::pv[] = {"other", "alto", "contralto",
		"soprano", "bass", "baritone", "tenor"};

void Singer::Set()
{
	Worker::Set();
	cout << "enter number:\n";
	int i= 0;
	for(i=0; i<Vtypes; i++)
	{
		cout << i << ": " << pv[i] << " ";
		if(i % 4 == 3)
			cout << endl;
	}
	if(i %4 != 0)
		cout <<endl;
	while(cin >> voice &&(voice < 0 || voice >= Vtypes))
		cout << "Please enter a value >= 0 and  <" << Vtypes << endl;
	while(cin.get() != '\n')
		continue;
}
void Singer::Show() const
{
	cout << "Cateory waiter:\n:";
	Worker::Show();
	cout << "Vocal range: " << 	pv[voice] << endl;
}
