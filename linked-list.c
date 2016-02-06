
// includes
#include <stddef.h>	        // NULL == /0
#include <stdio.h>			// printf
#include <stdlib.h>		  	// exit, malloc
#include "linked-list.h"
#include "bufferHelper.h"

// global variables
//struct NODE *head;


// this method will be responsible for printing out the
// data field value associated with each node in the list
void printList(struct NODE *head)
{
	printf("\n===== printList() ===== \n");

	// create a local pointer to walk the list
	struct NODE *current = head;

	if(isListEmpty(head)) // if true -- list is empty... print it
	{
		printf("=====/end printList() ===== \n");
	}
	else
	{
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
	}

	printf("=====/end printList() ===== \n");
}

int isListEmpty(struct NODE *_head)
{
	///printf("\n===== isListEmpty() ===== \n");

	///printf("_head: %p \n", _head);

	int flag = 0; // = 0 not empty

	if (_head == NULL)
	{
		// list is empty
		///puts("_head == null ");
		flag = 1;
		printf(">>> list is currently empty \n");
	}
	else
	{
		///puts("_head != null ");
		flag = 0;
	}

	///printf("=====/end isListEmpty() ===== \n");
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
		(*_head) = newest; // link ! -- now 1 node in list
	}
	else // else there is at least 1 node in list, append to end
	{
		// grab the memory address of the first node in list
		struct NODE *current = *_head;

		// iterate to end of list then link node
		while (current->next != NULL)
		{
			current = current->next; // current->next grabs a mem addr
		}
		// now at end of list
		//current->
		current->next = newest;
	}

	printf("=====/end enqueue() ===== \n\n");
}

// if using dequeue in a while loop, make sure while loop is controlled by
// is ListEmpty -- else if only calling dequeue once, need to watch for
// case where list is empty
// else -- when list is NOT empty but head->next = NULL -- we can't return
// that new node because when head-> next is NULL -- 1 node "head" in list..
// we are the tail
struct NODE *dequeue(struct NODE **_head)
{
	printf("\n===== dequeue() ===== \n");
	printf("*_head: %p \n", *_head);

	struct NODE *returningNode = NULL;

	if(isListEmpty(*_head)) // no nodes in list
	{
		puts("$ IF $ ");
		returningNode = NULL;
	}
	else if ((*_head)->next != NULL) // at least 2 nodes in list
	{
		puts("$ ELSE IF $ ");
		// fetch node and move head pointer to next node -- "dequeue"
		returningNode = (*_head);
		(*_head) = (*_head)->next;   // necessary for final dequeue
	}
	else // 1 node in list
	{
		puts("$ ELSE $ ");
		returningNode = (*_head);
		(*_head) = (*_head)->next; // now list will be empty
	}

	//printf("returning this node ~> %s \n", returningNode->data);
	//printf("returning this node ~> %p \n", returningNode->next);

	return returningNode;
}

int getListSize(struct NODE *head)
{
	printf("\n===== getListSize() ===== \n");
	int nodeCount = 0;

	// create a local pointer to walk the list
	struct NODE *current = head;

	if(isListEmpty(head))
		return nodeCount;

	// loop through each node in the list and print the value
	while (current->next != NULL)
	{
		current = current->next;
		nodeCount++;
	}

	nodeCount++; // for case when last node in list and curent->next==NULL

	return nodeCount;
}
