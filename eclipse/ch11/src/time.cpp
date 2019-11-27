/*
 * time.cpp
 *
 *  Created on: 2019Äê11ÔÂ8ÈÕ
 *      Author: dl
 */
#include <iostream>
#include "time.h"
Time::Time(){
	hours = minutes = 0;
}

Time :: Time(int h, int m){
	hours = h;
	minutes = m;
}

void Time::AddMin(int m){
	minutes += m;
	hours += minutes / 60;
	minutes %= 60;
}

void Time::AddHr(int h){
	hours += h;
}

void Time::Reset(int h, int m){
	hours = h;
	minutes = m;
}
 Time Time::Sum(const Time &t)const{
	 Time sum;
	 sum.minutes = minutes + t.minutes;
	 sum.hours = hours + t.hours + sum.minutes / 60;
	 sum.minutes %= 60;
	 return sum;
 }
 Time Time::operator +(const Time &t)const{
	 Time sum;
	 sum.minutes = minutes + t.minutes;
	 sum.hours = hours + t.hours + sum.minutes / 60;
	 sum.minutes %= 60;
	 return sum;
 }
 Time Time::operator -(const Time &t)const{
	 Time diff;
	 int tot1, tot2;
	 tot1 = t.minutes + 60 * t.hours;
	 tot2 = minutes + 60 * hours;
	 diff.minutes = (tot2 - tot1) % 60;
	 diff.hours = (tot2 - tot1) / 60;
	 return diff;
 }

 Time Time::operator * (double mul) const
 {
	 Time result;
	 long totalminutes = (hours *60 + minutes) * mul;
	 result.minutes = totalminutes % 60;
	 result.hours = totalminutes / 60;
	 return result;
 }

 Time operator*(double m, const Time&t){
	 Time result;
	 long totalminutes = (t.hours *60 + t.minutes)*m;
	 result.minutes = totalminutes % 60;
	 result.hours = totalminutes / 60;
	 return result;
 }
 void Time::show()const{
	 std::cout << hours << " hours, " << minutes << " minutes";
 }

 ostream & operator << (ostream &os, const Time &t){
	 os << t.hours << " hours, " << t.minutes << " minutes";
	 return os;
 }
