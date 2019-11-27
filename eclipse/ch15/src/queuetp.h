/*
 * queuetp.h
 *
 *  Created on: 2019Äê11ÔÂ26ÈÕ
 *      Author: dl
 */

#ifndef QUEUETP_H_
#define QUEUETP_H_

template <typename Item>
class QueueTp
{
private:
	enum{Q_SIZE = 10};
	class Node
	{
	public:
		Item item;
		Node *next;
		Node(const Item &i) : item(i),next(0){}
	};
	Node *front;
	Node *rear;
	int items;
	const int qsize;
	QueueTp(const QueueTp &q) : qsize(0){}
	QueueTp & operator=(const QueueTp *q){return *this;}
public:

};



#endif /* QUEUETP_H_ */
