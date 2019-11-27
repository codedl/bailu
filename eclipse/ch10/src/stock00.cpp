/*
 * stock00.cpp
 *
 *  Created on: 2019Äê11ÔÂ1ÈÕ
 *      Author: dl
 */
#include <iostream>
#include "stock00.h"

Stock::Stock(){
	std::cout << "default construcor called\n";
	company = "no name";
	shares = 0;
	share_val = 0.0;
	total_val = 0.0;
}

Stock::Stock(const std::string &co, long n, double pr){
	std::cout << "constructor using " << co << " called\n";
	company = co;
	if(n <0){
			std::cout << "number of shares can't negative;" << company
					<< "share set to 0.\n";
			shares = 0;
		}else shares = n;
		share_val = pr;
		set_tot();
}

Stock::~Stock(){
	std::cout << "Bye, " << company << "!\n";
}

void Stock::acquire(const std::string &co, long n, double pr){
	company = co;
	if(n <0){
		std::cout << "number of shares can't negative;" << company
				<< "share set to 0.\n";
		shares = 0;
	}else shares = n;
	share_val = pr;
	set_tot();
}

void Stock::buy(long num,double price){
	if(num < 0){
		std::cout << "number of shares can't negative;"
				<< "transaction is aborted \n";
	}else {
		shares += num;
		share_val = price;
		set_tot();
	}
}

void Stock::sell(long num, double price){
	using std::cout;
	if(num < 0){
		std::cout << "number of shares can't negative;"
				<< "transaction is aborted \n";
	}else if(num > shares){
		std::cout << "you can't sell more than you have!"
				<< "transaction is aborted!\n";
	}else{
		shares -= num;
		share_val = price;
		set_tot();
	}
}

void Stock::update(double price){
	share_val = price;
	set_tot();
}

void Stock::show ()  const
{
	using std::cout;
	using std::ios_base;
	ios_base::fmtflags orig = cout.setf(ios_base::fixed, ios_base::floatfield);
	std::streamsize prec = cout.precision(3);
	std::cout << "Company: " << company
			<< " Shares: " << shares << '\n'
			<< " Share Price: " << share_val << '\n'
			<< " Total Worth: " << total_val << '\n';
}

const Stock & Stock::topval(const Stock &s)const{
	if(s.total_val > this->total_val)
		return s;
	else
		return *this;
}

