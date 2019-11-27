/*
 * namespace.h
 *
 *  Created on: 2019Äê11ÔÂ1ÈÕ
 *      Author: dl
 */

#ifndef NAMESPACE_H_
#define NAMESPACE_H_
#include <string>
namespace pers{
	struct Person{
		std::string fname;
		std::string lname;
	};
	void getPerson(Person &);
	void showPerson(const Person &);
}

namespace debts{
	using namespace pers;
	struct Debt{
		Person name;
		double amount;
	};
	void getDebt(Debt &);
	void showDebt(const Debt &);
	double sumDebts(const Debt ar[], int n);
}

#endif /* NAMESPACE_H_ */
