/*
 * time.h
 *
 *  Created on: 2019Äê11ÔÂ8ÈÕ
 *      Author: dl
 */

#ifndef TIME_H_
#define TIME_H_
#include<iostream>
using std::ostream;
class Time{
private :
	int hours;
	int minutes;
public:
	Time();
	Time(int h, int m=0);
	void AddMin(int m);
	void AddHr(int h);
	void Reset(int h=0, int m=0);
	Time Sum(const Time &t) const;
	Time operator+(const Time &t) const;
	Time operator-(const Time &t) const;
	Time operator*(double n) const;
	friend Time operator*(double m, const Time &t);
	void show() const;
	friend ostream & operator << (ostream &os, const Time &t);
};




#endif /* TIME_H_ */
