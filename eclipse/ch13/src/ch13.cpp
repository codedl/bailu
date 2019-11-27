//============================================================================
// Name        : ch13.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "tabtenn0.h"
#include "acctabc.h"
#include "dma.h"
using namespace std;
void BrassTest();
void VertualTest();
void dmaTest();
int main() {
//	VertualTest();
	dmaTest();
	return 0;
}
void BrassTest()
{
	Brass Piggy("porcelot pigg", 381299, 4000);
	BrassPlus Hoggy("Horatio Hogg", 382288, 3000.00);
	Piggy.ViewAcct();
	cout << endl;
	Hoggy.ViewAcct();
	cout << endl;
	cout << "Depositing $1000 into the Hogg Account:\n";
//	Hoggy.Deposit(1000);
	cout << "New Balance: $" << Hoggy.Balance() << endl;
	cout << "Withdrawing $4200 from the Pigg Account:\n";
	Piggy.Withdraw(4200);
	cout << "Pigg account balance: $" << Piggy.Balance() << endl;
	cout << "Withdrawing $4200 form the Hogg Account:\n";
	Hoggy.Withdraw(4200);
	Hoggy.ViewAcct();
}
void VertualTest()
{
	const int CLIENTS = 4;
	AcctABC * p_clients[CLIENTS];
	string temp;
	long tempnum;
	double tempbal;
	char kind;
	for(int i=0; i<CLIENTS; i++)
	{
		cout << "enter client's name:";
		getline(cin, temp);
		cout << "enter client's account number";
		cin >> tempnum;
		cout << "enter opening balance: $";
		cin >> tempbal;
		cout << "enter 1 for brass account or "
				<< "2 for brassplus account:";
		while(cin >> kind && (kind != '1' && kind != '2'))
			cout << "enter either 1 or 2";
		if(kind == '1')
			p_clients[i] = new Brass(temp, tempnum, tempbal);
		else
		{
			double tmax, trate;
			cout << "enter the overdraft limit: $";
			cin >> tmax;
			cout << "enter the 	interest rate "
					<< "as a decimal fraction: ";
			cin >> trate;
			p_clients[i] = new BrassPlus(temp, tempnum, tempbal, tmax, trate);

		}
		while(cin.get() != '\n')
			continue;
	}

	cout << endl;
	for(int i=0; i<CLIENTS; i++)
	{
		p_clients[i] -> ViewAcct();
		cout << endl;
	}
	for(int i=0; i<CLIENTS; i++)
	{
		delete p_clients[i];
	}
	cout << "Done.\n";

}

void dmaTest()
{
	baseDMA shirt("prot", 8);
	lacksDMA balloon("red","Blimp",4);
	hasDMA map("mercator", "Buffalo", 5);

	cout << "display baseDMA object:\n";
	cout << shirt << endl;

	cout << "display lasckDMA object:\n";
	cout << balloon << endl;

	cout << "display hasDMA object:\n";
	cout << map << endl;

	lacksDMA balloon2(balloon);
	cout << "lacksDMA result copy:\n";
	cout << balloon2 << endl;

	hasDMA map2;
	map2 = map;
	cout << "result of hasDMA assignment:\n";
	cout << map2 << endl;
}
