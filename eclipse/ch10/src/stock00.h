/*
 * stock00.h
 *
 *  Created on: 2019年11月1日
 *      Author: dl
 */

#ifndef STOCK00_H_
#define STOCK00_H_

#include <string>//使用名称空间
class Stock{
private:
	std::string company;
	long shares;
	double share_val;
	double total_val;
	void set_tot(){total_val = shares*share_val;}

public:
	Stock();
	Stock(const std::string &co, long n=0, double pr=0);
	~Stock();
	void acquire(const std::string &co, long n, double pr);
	void buy(long num, double price);
	void sell(long num, double price);
	void update(double price);
	void show() const;
	const Stock & topval(const Stock &s)const;
};



#endif /* STOCK00_H_ */
