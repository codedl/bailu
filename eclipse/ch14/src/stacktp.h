/*
 * stacktp.h
 *
 *  Created on: 2019Äê11ÔÂ26ÈÕ
 *      Author: dl
 */

#ifndef STACKTP_H_
#define STACKTP_H_

template <typename Type>
class Stack
{
private:
	enum {SIZE = 10};
	int stacksize;
	Type *items;
	int top;
public:
	explicit Stack(int size = SIZE);
	Stack(const Stack & s);
	~Stack(){delete [] items;}
	bool isEmpty();
	bool isFull();
	bool push(const Type &item);
	bool pop(Type &item);
	Stack& operator= (const Stack &s);
};
template <typename T> Stack<T>::Stack(int size) : stacksize(size),top(0)
{
	items = new T[size];
}
template <typename T> Stack<T>::Stack(const Stack &s)
{
	stacksize = s.stacksize;
	top = s.top;
	items = new T[s.stacksize];
	for(int i=0; i<stacksize; i++)
		items[i] = s.items[i];
}
template <typename T>bool Stack<T>::isEmpty()
{
	return top == 0;
}
template <typename T>bool Stack<T>::isFull()
{
	return top == SIZE;
}
template <typename T>bool Stack<T>::push(const T &item)
{
	if(top < SIZE)
	{
		items[top++] = item;
		return true;
	}
	else
		return false;
}
template <typename T>bool Stack<T>::pop( T &item)
{
	if(top > 0)
	{
		item = items[--top];
		return true;
	}
	else
		return false;
}
template <typename T> Stack<T>& Stack<T>::operator=(const Stack<T> &s)
{
	if(this == &s)
		return *this;
	delete []items;
	stacksize = s.stacksize;
	top = s.top;
	items = new T[s.stacksize];
	for(int i=0; i<top; i++)
		items[i] = s.items[i];
	return *this;
}

template <template <typename T> class Thing>
class Crab
{
private:
	Thing<int> s1;
	Thing<double> s2;
public:
	Crab(){};
	bool push(int a, double x) {return s1.push(a) && s2.push(x);}
	bool pop(int &a, double &x) {return s1.pop(a) && s2.pop(x);}
};
#endif /* STACKTP_H_ */
