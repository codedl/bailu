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
	QueueTp(int qs=Q_SIZE);
	~QueueTp();
	bool isEmpty()const
	{
		return items == 0;
	}
	bool isFull()const
	{
		return items == qsize;
	}
	int queuecount()const
	{
		return qsize;
	}
	bool enqueue(const Item &item);
	bool dequeue(Item &item);
};
template <typename T> QueueTp<T> ::QueueTp(int qs) : qsize(qs)
		{
			front = rear = 0;
			items = 0;
		}
template <typename T> QueueTp<T> :: ~QueueTp()
{
	Node *temp;
	while(front != 0)
	{
		temp = front;
		front = front->next;
		delete temp;
	}
}
template <typename T> bool QueueTp<T>::enqueue(const T & item)
		{
			if(isFull())
				return false;
			Node * node = new Node(item);
			items ++;
			if(front == 0)
				front = node;
			else
				rear -> next = node;
			rear = node;
			return true;
		}
template <typename Item> bool QueueTp<Item>::dequeue(Item &item)
		{
			if(front == 0)
				return false;
			items --;
			item = front->item;
			Node *temp = front;
			front = front -> next;
			delete temp;
			if(items == 0)
				rear = 0;
			return rear;
		}
#endif /* QUEUETP_H_ */
