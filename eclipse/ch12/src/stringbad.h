/*
 * stringbad.h
 *
 *  Created on: 2019Äê11ÔÂ19ÈÕ
 *      Author: dl
 */

#ifndef STRINGBAD_H_
#define STRINGBAD_H_

#include <iostream>
class StringBad
{
private:
	char *str;
	int len;
	static int num_strings;
public:
	StringBad();
	StringBad(const char *s);
	StringBad(const StringBad &st);
	~StringBad();
	friend std::ostream & operator << (std::ostream &os, const StringBad &st);
	StringBad & operator=(const StringBad &st);
	int length() const {return len;}
};


#endif /* STRINGBAD_H_ */
