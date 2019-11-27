/*
 * stonewt.h
 *
 *  Created on: 2019Äê11ÔÂ18ÈÕ
 *      Author: dl
 */

#ifndef STONEWT_H_
#define STONEWT_H_

class Stonewt
{
private:
	enum{Lbs_per_stn = 14};
	int stone;
	double pds_left;
	double pounds;
public:
	Stonewt(double lbs);
	Stonewt(int std,double lbs);
	Stonewt();
	~Stonewt();
	void show_lbs() const;
	void show_stn() const;
	operator int() const;
	operator double() const;
};



#endif /* STONEWT_H_ */
