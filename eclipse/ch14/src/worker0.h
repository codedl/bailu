/*
 * worker0.h
 *
 *  Created on: 2019Äê11ÔÂ25ÈÕ
 *      Author: dl
 */

#ifndef WORKER0_H_
#define WORKER0_H_
#include <string>
using std::string;
class Worker
{
private:
	string fullname;
	long id;
public:
	Worker():fullname("no one"),id(0L){}
	Worker(const string &name, long n):fullname(name), id(n){}
	virtual ~Worker() = 0;
	virtual void Set() = 0;
	virtual void Show() const = 0;
};

class Waiter : public Worker
{
private:
	int panache;
public:
	Waiter() : Worker(),panache(0){};
	Waiter(const string &n, long i, int p=0) : Worker(n, i),panache(p){}
	Waiter(const Worker &w, int p=0 ) : Worker(w),panache(p){}
	void Set();
	void Show() const;
};
class Singer : public Worker
{
protected:
	enum{other, alto, contralto, soprano, bass, baritone, tenor};
	enum{Vtypes = 7};
private:
	static char *pv[Vtypes];
	int voice;
public:
	Singer() : Worker(),voice(other){}
	Singer(const string &s, long n, int v = other) : Worker(s,n),voice(v){}
	Singer(const Worker &w, int v=other) : Worker(w),voice(v){}
	void Set();
	void Show() const;
};
#endif /* WORKER0_H_ */
