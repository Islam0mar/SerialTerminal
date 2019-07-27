#ifndef QUE_H
#define QUE_H

#include <stdbool.h>
#include <stdlib.h>


typedef struct elementtype
{
	char* string;
	// union
	// {
	//
	// }info;
}ElementType;

typedef ElementType QueueEntry;

typedef struct queuenode
{
		QueueEntry entry;
		struct queuenode *next;
}QueueNode;

typedef struct queue
{
	QueueNode *head;
	QueueNode *tail;
}Queue;


QueueNode *MakeQueueNode(QueueEntry item)
{
	QueueNode *p;

	p=(QueueNode *)malloc(sizeof(QueueNode));
	if(p!=NULL)
	{
		p->entry=item;
		p->next=NULL;
	}
	return p;
}

bool AddToTail(QueueEntry item, Queue *pq)
{
	QueueNode *p=MakeQueueNode(item);
	if(p == NULL)
	{
		return false;
	}
	if(pq->head==NULL && pq->tail==NULL)
	{
		pq->head=p;
		pq->tail=p;
		return true;
	}
	pq->tail->next=p;
	pq->tail=p;
	return true;
}

int QueueEmpty(Queue *pq)
{
	return pq->head==NULL;
}

void CreateQueue (Queue *pq)
{
	pq->head=pq->tail=NULL;
}

int	QueueSize(Queue *pq){
  int x;
  QueueNode *ptrs=pq->head;
  for(x=0; ptrs; ptrs=ptrs->next)
    x++;
  return x;
}

void ClearQueue(Queue *pq)
{
	QueueNode *p=pq->head;
	while(p)
	{
		p=p->next;
		free(pq->head);
		pq->head=p;
	}
	pq->tail=pq->head;
}

#endif /* QUE_H */
