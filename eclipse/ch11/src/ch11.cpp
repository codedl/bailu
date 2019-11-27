//============================================================================
// Name        : ch11.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "time.h"
#include "stonewt.h"
using namespace std;
void time0_test();
void time1_test();
void stone_test();
int main() {
//	time0_test();
//	time1_test();
	stone_test();
	return 0;
}
void time0_test(){
	Time planning;
	Time coding(2,40);
	Time fixing(5,55);
	Time total;

	cout << "planning time = ";
	planning.show();
	cout << endl;

	cout << "coding time = ";
	coding.show();
	cout << endl;

	cout << "fixing time = ";
	fixing.show();
	cout << endl;

//	total = coding.Sum(fixing);
	total = coding + fixing;
	cout << "coding.Sum(fixing) = ";
	total.show();
	cout << endl;

}
void time1_test(){
	Time weeding(4,35);
	Time waxing(2, 47);
	Time total;
	Time diff;
	Time adjusted;

	cout << "wedding time = ";
	weeding.show();
	cout << endl;

	cout << "total work time = ";
	total = weeding + waxing;
	total.show();
	cout << endl;

	diff = weeding - waxing;
	cout << "weeding time - waxing time = ";
	diff.show();
	cout << endl;

//	adjusted = total * 1.5;
	adjusted = 1.5 * total;
	cout << "adjusted work time = " << adjusted;
//	adjusted.show();
//	cout << adjusted;
	cout << endl;

}

void stone_test()
{
	Stonewt incognito = 275;
	Stonewt wolfe(285.7);
	Stonewt taft(21,8);
	cout << "The celebrity weighed ";
	incognito.show_stn();
	cout << "The detective weighed ";
	wolfe.show_stn();
	cout << "The President weighed ";
	taft.show_stn();
	incognito = 276.8;
	taft = 325;

	Stonewt poppins(9,2.8);
	double p_wt = poppins;
	cout << "convert to double =>";
	cout << "Poppins: " << p_wt << " pounds\n";
	cout << "convert to int =>";
	cout << "Poppins: " << int(poppins) << " pounds.\n";
}
