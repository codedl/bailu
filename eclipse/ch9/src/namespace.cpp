/*
 * namespace.cpp
 *
 *  Created on: 2019Äê11ÔÂ1ÈÕ
 *      Author: dl
 */
#include <iostream>
#include "namespace.h"
namespace pers{
	using std::cin;
	using std::cout;
	void getPerson(Person & rp){
		cout << "enter first name: ";
		cin >> rp.fname;
		cout << "enter last name: ";
		cin >> rp.lname;
	}
	void showPerson(const Person &rp){
		std::cout << rp.lname << ", _" << rp.fname;
	}
}

namespace debts{
	void getDebt(Debt &rd){
		getPerson(rd.name);
		std::cout << "enter debt: ";
		cin >> rd.amount;
	}
	void showDebt(const Debt &rd){
		showPerson(rd.name);
		std::cout << ": $" << rd.amount << std::endl;
	}

	double sumDebts(const Debt ar[], int n){
		double total = 0;
		for(int i=0; i<n; i++)
			total += ar[i].amount;
		return total;
	}
}



