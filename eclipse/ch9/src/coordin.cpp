/*
 * coordin.cpp
 *
 *  Created on: 2019Äê10ÔÂ31ÈÕ
 *      Author: dl
 */
#include <iostream>
#include <cmath>
#include "coordin.h"
using namespace std;

polar rect_to_polar(rect xypos){
	polar answer;
	answer.distance = sqrt(xypos.x * xypos.x + xypos.y * xypos.y);
	answer.angle = atan2(xypos.y, xypos.x);
	return answer;
}

void show_polar(polar dapos){
	const double Rad_to_reg = 57.29577951;
	cout << "distance:" << dapos.distance;
	cout << "angle:" << dapos.angle * Rad_to_reg;
	cout << "degress\n";
}

void oil(int x){
	int texas = 5;
	cout << "in oil: texas = " << texas << "&texas:" << &texas << endl;
	cout << "in oil: x = " << x << "&x:" << &x << endl;
	{
		int texas = 113;
		cout << "in block: texas = " << texas << "&texas:" << &texas << endl;
		cout << "in block: x = " << x << "&x:" << &x << endl;
	}
	cout << "post block: texas = " << texas << "&texas:" << &texas << endl;
}

extern int tom;
static int dick = 10;
int harry = 200;
void remote_access(){
	cout << "\nin coordin:\n" << "&tom:" << &tom << "&dick:" << &dick << "&harry:" << &harry <<endl;
}



