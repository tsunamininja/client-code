/*
 * linked-list.h
 *
 *  Created on: Jan 30, 2016
 *      Author: joe
 */

#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_


// includes

// defines

// structures 'nodes'
struct NODE {
	unsigned char *data;
	unsigned int dataLength;
	struct NODE *next;
};

// function definitions (API)
///struct NODE *createList();

void enqueue(struct NODE **_head,
				unsigned char *_data,
					unsigned int _dataLength);

void printList(struct NODE *head);

// createList

// push

// print







#endif /* LINKED_LIST_H_ */
