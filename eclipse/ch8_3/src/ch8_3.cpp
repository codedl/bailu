//============================================================================
// Name        : ch8_3.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cstring>
//#include <string.h>
struct job {
	char name[40];
	double salary;
	int floor;
};
template <typename T>void swaper(T &a, T &b);//函数模板
template <typename T>void swaper(T a[], T b[], int n);//函数模板重载
template <>void swaper<job> (job &j1, job &j2);//显示具体化
void show(int a[]);
void show(job &j);
using namespace std;
const int ArSize = 80;
char *left(const char*str, int n = 1);
unsigned long left(unsigned long num, unsigned ct);
template <typename T>void ShowArray(T arr[],int n);
template <typename T>void ShowArray(T* arr[],int n);
struct debts{
	char name[40];
	double amount;
};
int main() {
	/*char sample[ArSize];
	cout << "Enter a String:\n";
	cin.get(sample,ArSize);
	char *ps = left(sample, 4);
	cout << ps << endl;
	delete []ps;
	ps = left(sample);
	cout << ps << endl;
	delete []ps;

	char *trip = "hawaii";
	unsigned long n = 12345678;
	char *temp;
	for(int i=0; i<10; i++){
		cout << left(n,i) << endl;
		temp = left(trip,i);
		cout << temp <<endl;
		delete []temp;
	}*/
	int i = 10;
	int j = 20;
	cout << "i , j:" << i << "," << j << endl;
	cout << "use int to swap:" << endl;
	swaper(i,j);
	cout << "i , j:" << i << "," << j << endl;

	double x = 20.19;
	double y = 10.30;
	cout << "x,y:" << x << "," << y << endl;
	cout << "use double to swap:\n";
	swaper(x,y);
	cout << "x,y:" << x << "," << y << endl;

	int d1[] = {0,7,0,4,1,7,7,6};
	int d2[] = {2,0,1,9,1,0,3,0};
	show(d1);
	show(d2);
	swaper(d1,d2,8);
	cout << "use int to swaper overload\n";
	show(d1);
	show(d2);

	cout.precision(2);
	cout.setf(ios::fixed, ios::floatfield);
	job sue = {"susan",73000.60,7};
	job sid = {"sidney",78060.72,9};
	cout << "before swaper:" ;
	show(sue);
	show(sid);
	swaper(sue,sid);
	cout<<"after swaper:";
	show(sue);
	show(sid);

	int things[6] = {13,31,103,301,310,130};
	struct debts mr_E[3] = {
			{"ima",2400.0},
			{"ura",1300.0},
			{"iny",1800.0}
	};
	double *pd[3];
	for(int i=0; i<3; i++)
		pd[i] = &mr_E[i].amount;

	ShowArray(things, 6);
	ShowArray(pd, 3);

	return 0;
}

template <typename T>void ShowArray(T arr[], int n){
	cout << "template A\n";
	for(int i=0; i<n; i++)
		cout << arr[i] << ' ';
	cout << endl;
}

template <typename T>void ShowArray(T *arr[], int n){
	cout << "template B\n";
	for(int i=0; i<n; i++)
		cout << *arr[i] << ' ';
	cout << endl;
}

char *left(const char *str, int n){
	if(n < 0)
		n=0;
	int length = strlen(str);
	n = (n < length) ? n : length;
	char *p = new char[n+1];
	int i;
	for(i=0; i<n&&str[i]; i++)
		p[i] = str[i];
	while(i<=n)
		p[i++] = '\0';
	return p;
}

unsigned long left(unsigned long num, unsigned ct){
	if(num ==0 || ct ==0)
		return 0;
	unsigned digits = 1;
	unsigned long n = num;
	while(n /= 10)
		digits ++;
	if(digits > ct){
		ct = digits - ct;
		while(ct --)
			num /= 10;
		return num;
	}else return num;

}

template <typename T> void swaper(T &a, T &b){
	T temp;
	temp = a;
	a = b;
	b =temp;

}

template <typename T>void swaper(T a[], T b[], int n){
	T temp;
	for(int i=0; i<n; i++){
		temp = a[i];
		a[i] = b[i];
		b[i] = temp;
	}
}

 void show(int a[]){
//	int length = sizeof a / sizeof a[0];
	for(int i =0; i<8; i++)
		cout << a[i];
	cout << endl;
}

 void show(job &j){
	 cout << j.name << j.salary << j.floor << endl;
 }

 template <>void swaper<job> (job &j1, job &j2){
	 double d = j1.salary;
	 j1.salary = j2.salary;
	 j2.salary = d;

	 int i = j1.floor;
	 j1.floor = j2.floor;
	 j2.floor = i;
 }
