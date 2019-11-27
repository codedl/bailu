/*
 * coordin.h
 *
 *  Created on: 2019Äê10ÔÂ31ÈÕ
 *      Author: dl
 */

#ifndef COORDIN_H_
#define COORDIN_H_

struct polar{
	double distance;
	double angle;
};

struct rect{
	double x;
	double y;
};

polar rect_to_polar(rect xypos);
void show_polar(polar dapos);
void oil(int x);

#endif /* COORDIN_H_ */
