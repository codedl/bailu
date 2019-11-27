/*
 * arraytp.h
 *
 *  Created on: 2019Äê11ÔÂ26ÈÕ
 *      Author: dl
 */

#ifndef ARRAYTP_H_
#define ARRAYTP_H_

#include <iostream>
#include <cstdlib>

template <typename T, int n>
class ArrayTp
{
private:
	T arr[n];
public:
	ArrayTp(){};
	explicit ArrayTp(const T& t);
	virtual T& operator[] (int i);
	virtual T operator[] (int i)const;
};

template <typename T, int n> ArrayTp<T,n>::ArrayTp(const T& t)
		{
			for(int i=0; i<n; i++)
				arr[i] = t;
		}
template <typename T, int n> T& ArrayTp<T,n>::operator[](int i)
{
	if(i<0 || i>= n)
	{
		std::cerr << "out of range" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return arr[i];
}
template <typename T, int n> T ArrayTp<T,n>::operator [](int i)const
{
	if(i<0 || i>= n)
	{
		std::cerr << "out of range" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return arr[i];
}

#endif /* ARRAYTP_H_ */
