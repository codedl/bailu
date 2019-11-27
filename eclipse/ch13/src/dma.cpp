/*
 * dma.cpp
 *
 *  Created on: 2019Äê11ÔÂ23ÈÕ
 *      Author: dl
 */
#include "dma.h"
#include <cstring>
using  std::strlen;
using  std::strncpy;
baseDMA::baseDMA(const char *l, int r)
{
	label = new char[std::strlen(l) + 1];
	std::strcpy(label,l);
	rating = r;
}
baseDMA::baseDMA(const baseDMA & rb)
{
	label = new char[std::strlen(rb.label) + 1];
	std::strcpy(label,rb.label);
	rating = rb.rating;
}
baseDMA::~baseDMA()
{
	delete[]label;
}
baseDMA & baseDMA::operator =(const baseDMA & rb)
{
	if(this == &rb)
		return *this;
	delete [] label;
	label = new char[std::strlen(rb.label) + 1];
	std::strcpy(label,rb.label);
	rating = rb.rating;
	return *this;
}
std::ostream & operator << (std::ostream &os, const baseDMA &rb)
{
	os << "Label: " << rb.label << std::endl;
	os << "Rating: " << rb.rating << std::endl;
	return os;
}

lacksDMA::lacksDMA(const char *c, const char*l, int r) : baseDMA(l, r)
{
	std::strncpy(color,c,39);
	color[39] = '\0';
}
lacksDMA::lacksDMA(const char *c, const baseDMA &rb) : baseDMA(rb)
{
	std::strncpy(color,c,39);
	color[39] = '\0';
}
std::ostream & operator<< (std::ostream &os, const lacksDMA &rl)
{
	os << (const baseDMA&)rl;
	os << "COLOR: " << rl.color << std::endl;
	return os;
}

hasDMA::hasDMA(const char *s, const char *l,int r) : baseDMA(l, r)
{
	style = new char[std::strlen(s) + 1];
	std::strcpy(style,s);
}
hasDMA::hasDMA(const char *s, const baseDMA &rs) : baseDMA(rs)
{
	style = new char[std::strlen(s) + 1];
	std::strcpy(style,s);
}
hasDMA::hasDMA(const hasDMA &rs) : baseDMA(rs)
{
	style = new char[std::strlen(rs.style) + 1];
	std::strcpy(style, rs.style);
}
hasDMA::~hasDMA()
{
	delete[]style;
}
hasDMA & hasDMA::operator= (const hasDMA& rh)
{
	if(this == &rh)
		return *this;
	baseDMA::operator =(rh);
	delete [] style;
	style = new char[std::strlen(rh.style) + 1];
	std::strcpy(style,rh.style);
	return *this;
}
std::ostream & operator<< (std::ostream & os, const hasDMA &hs)
{
	os << (const baseDMA &)hs;
	os << "Style: " << hs.style << std::endl;
}
