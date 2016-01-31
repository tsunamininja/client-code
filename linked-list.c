
// includes
#include <stddef.h>	        // NULL == /0
#include <stdio.h>			// printf
#include <stdlib.h>		  	// exit, malloc
#include "linked-list.h"
#include "bufferHelper.h"

// global variables
//struct NODE *head;

// functions implemented
//struct NODE *createList()
//{
	// create head node
//	return head;
//}

// this method will be responsible for printing out the
// data field value associated with each node in the list
void printList(struct NODE *head)
{
	printf("\n===== printList() ===== \n");
	//printf("head -> hostName: %s \n", head->hostName);
	//printf("head -> size: %u \n", head->size);
	//printf("------------------------\n");


	// create a local pointer to walk the list
	struct NODE *current = head;

	if(current == NULL)
	{
		printf("list is empty... exiting... \n");
		exit(1);
	}

	// now the value of current = = the value of head
	// they each contain the memory address of the head variable

	// loop through each node in the list and print the value
	while (current->next != NULL)
	{
		// print out the first data field in the list

		// send too
		//sendQuery(current->hostName, current->size, NULL);
		//sleep(3);
		printf("current ~ hostName: %s \n", current->data);

		// at this point, the current pointer still points at head
		// we need to modify the value of current, by assigning it
		// the value associated with its 'next' member
		current = current->next;

		// now the current variable value has been modified and now
		// contains a new memory address corresponding to the next
		// node that had been linked in previously

		// if the value of current = NULL, as would be the case
		// when the end of the list was reached and the next data member
		// had been changed to NULL, then we are at the end of the list
		// ie "node1->next = NULL;"
	}

	// current->next == NULL here.. end of list but we can still print this node
	printf("tail node ~ hostName: %s \n", current->data);
	//printf("tail node ~ size: %u \n", current->size);

	printf("=====/end printList() ===== \n");
}


int isListEmpty(struct NODE *_head)
{
	int flag = 0; // = 0 not empty

	if (_head == NULL)
	{
		// list is empty
		flag = 1;
		printf("\n[!] list is currently empty \n");
	}
	else
	{
		flag = 0;
	}

	return flag;
}


/*
 * push is more of a stack operation, LIFO
 *
 * we want enqueue (FIFO)
 *
 * *_head  -- dereferences once -- retrieves -VALUE- at memory addr / ptr
 *
 * in our case by deref once, we get value at stack variable address.
 * which is currently NULL, we will make this non-null and thus attach head
 * to the list
 */
void enqueue(struct NODE **_head,
				unsigned char *_data,
					unsigned int _dataLength)
{
	printf("\n===== enqueue() ===== \n");
	printf("_data> %s \n", _data);
	printf("_dataLength -> %u \n", _dataLength);

	// create a node to represent new guy
	struct NODE *newest = malloc(sizeof(struct NODE));

	// allocate room for *data member
	newest->data = malloc(sizeof(unsigned char)*_dataLength);

	// copy data string into new member
	copyString(newest->data, _data, _dataLength);

	// add _dataLength STRING_NODE member
	newest->dataLength = _dataLength;
	newest->next = NULL;

	// check to see if list is empty
	if (isListEmpty(*_head)) // is empty - return 1 == if = true
	{
		printf("*_head is null, linking to create 1 node \n");
		*_head = newest; // link ! -- now 1 node in list
	}
	else // else there is at least 1 node in list, append to end
	{
		// grab the memory address of the first node in list
		struct NODE *current = *_head;

		// iterate to end of list then link node
		while (current->next != NULL)
		{
			printf("current is not null \n");
			current = current->next; // current->next grabs a mem addr
		}
		// now at end of list
		//current->
		current->next = newest;
	}

	printf("=====/end enqueue() ===== \n\n");
}

struct NODE *dequeue(struct NODE **_head)
{
	printf("\n===== dequeue() ===== \n");

	// create a local pointer to walk the list
	struct NODE *current = *_head;

	// fetch node and move head pointer to next node
	*_head = current->next;

	printf("=====/end dequeue() ===== \n\n");

	return current;
}
