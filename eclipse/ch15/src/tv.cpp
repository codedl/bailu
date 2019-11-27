/*
 * tv.cpp
 *
 *  Created on: 2019Äê11ÔÂ26ÈÕ
 *      Author: dl
 */
#include <iostream>
#include "tv.h"

bool Tv::volup()
{
	if(volume < MaxVal)
	{
		volume ++;
		return true;
	}
	else
		return false;
}

bool Tv::voldown()
{
	if(volume > 0)
	{
		volume --;
		return true;
	}
	else
		return false;
}

void Tv::chanup()
{
	if(channel < maxchannel)
		channel ++;
	else
		channel = 1;
}
void Tv::chandown()
{
	if(channel > 1)
		channel--;
	else
		channel = maxchannel;
}
void Tv::setting()const
{
	using std::cout;
	using std::endl;

	cout << "TV is " << (state == On? "On" : "Off") << endl;
	if(state == On)
	{
		cout << "Volume setting = " << volume << endl;
		cout << "Channel setting = " << channel << endl;
		cout << "Mode = " << (mode == Antenna? "antenna" : "cable") << endl;
		cout << "Input = " << (input == TV? "TV" : "DVD") << endl;
	}
}
