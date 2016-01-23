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
 * arguments -
 *
 * return value -
 *
 * comments - dnsQueryBuffer is the buffer returned that represents a complete
 * dns query buffer, "udp payload", which is passed to a socket.
 */
int createDnsQueryPacket(unsigned char **stdoutBuffArg,
							unsigned char *qnameArg)
{
	//*stdoutBuff = malloc(sizeof(unsigned char) * 5);
	//memcpy(*stdoutBuff, "hell", 5);

	// to keep track of put()'s
	int buffIndex = 0;

	struct DNS_HEADER *ptrStructDnsH;
	struct DNS_QUESTION *ptrStructDnsQ;

	/* create and fill in a DNS_HEADER struct */
	ptrStructDnsH = createDnsHeader(
						TRANSAC_ID,
						QUERYFLAGS,
						QDCOUNT,
						ANCOUNT_NULL,
						NSCOUNT_NULL,
						ARCOUNT_NULL
						);

	/* create and fill in a DNS_QUESTION struct */
	ptrStructDnsQ = createDnsQuestion(qnameArg, QTYPE_A, QCLASS_IN);

	printf("[-] Printing DNS Query Message Packet \n");
	printf("DNS Header Section \n");
	printf("id -> %u 		\n", ptrStructDnsH->id);
	printf("flags -> %u 	\n", ptrStructDnsH->flags);
	printf("qdcode -> %u    \n", ptrStructDnsH->qdcode);
	printf("ancount -> %u   \n", ptrStructDnsH->ancount);
	printf("nscount -> %u   \n", ptrStructDnsH->nscount);
	printf("arcount -> %u   \n", ptrStructDnsH->arcount);
	printf("---------------------------------- \n");

	printf("DNS Question Section \n");
	printf("question -> ");
	stringPrinter(ptrStructDnsQ->qname, strlen(ptrStructDnsQ->qname)+1);
	printf("\n");
	printf("type -> %u 		\n", ptrStructDnsQ->qtype);
	printf("class -> %u		\n", ptrStructDnsQ->qclass);

	// somehow return the address of a char buffer to *stdoutBuff..
	*stdoutBuffArg = malloc(sizeof(struct DNS_HEADER));

	// copy each value into *stdoutBuffArg array elements

	/* dns header section field member copy */
	putShort(*stdoutBuffArg, &(ptrStructDnsH->id), 		&buffIndex);
	putShort(*stdoutBuffArg, &(ptrStructDnsH->flags),   &buffIndex);
	putShort(*stdoutBuffArg, &(ptrStructDnsH->qdcode),  &buffIndex);
	putShort(*stdoutBuffArg, &(ptrStructDnsH->ancount), &buffIndex);
	putShort(*stdoutBuffArg, &(ptrStructDnsH->nscount), &buffIndex);
	putShort(*stdoutBuffArg, &(ptrStructDnsH->arcount), &buffIndex);

	/* dns question section field member copy */
	putString(*stdoutBuffArg,
					ptrStructDnsQ->qname,
						strlen(ptrStructDnsQ->qname)+1,
							&buffIndex);

	putShort(*stdoutBuffArg, &(ptrStructDnsQ->qtype),  &buffIndex);
	putShort(*stdoutBuffArg, &(ptrStructDnsQ->qclass), &buffIndex);

	return buffIndex ;
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
 * function name -
 *
 * arguments -
 *
 * return value -
 *
 *
 */
struct DNS_HEADER *createDnsHeader (
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
struct DNS_QUESTION *createDnsQuestion(unsigned char *qNameArg,
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


/* utility functions below */

/*
 * function name - getQueryNameLength
 *
 * arguments -
 *
 * return value -
 *
 * comments - this function receives a compliant dns query name string, ie,
 * www.hello.com, and returns the length of it.
 *
 * Really only used for parsing incoming DNS responses
 *
 */
unsigned int getQueryNameLength(unsigned char* qNameArg,
									unsigned int *qNameLen)
{
	if(debug)
	{
		printf("==============DEBUG===============  \n");
		printf("function name - getQueryNameLength  \n");
		printf("qNameArg-> %s 		\n", qNameArg);
		printf("qNameLen> %u 		\n", *qNameLen);
		printf("----------------------------------  \n");
	}

	unsigned int labelCount   = 0;
	unsigned int labelLen1 	  = 0;
	unsigned int labelLen2	  = 0;
	unsigned int labelLen3 	  = 0;
	unsigned int labelLen4    = 0;
	unsigned int queryNameLen = 0;

	/* retrieve the label Len value for current octet */
	labelLen1 = qNameArg[0];
	if(labelLen1 > 0)
	{
		/* update value of labelCount and queryNameLen */
		labelCount++;
		queryNameLen = labelLen1+labelCount;
	}
	/* end of query... root TLD . reached.. */
	else
	{
		labelCount++;
		queryNameLen = labelLen1+labelLen2+labelLen3+labelLen4+labelCount;
	}

	/* retrieve the next label Len value for next octet */
	labelLen2 = qNameArg[queryNameLen];

	if(labelLen2 > 0)
	{
		/* update value of labelCount and queryNameLen */
		labelCount++;
		queryNameLen = labelLen1+labelLen2+labelCount;
	}
	/* end of query... root TLD . reached.. */
	else
	{
		labelCount++;
		queryNameLen = labelLen1+labelLen2+labelLen3+labelLen4+labelCount;
	}

	/* retrieve the next label Len value for next octet */
	labelLen3 = qNameArg[queryNameLen];

	if(labelLen3 > 0)
	{
		/* update value of labelCount and queryNameLen */
		labelCount++;
		queryNameLen = labelLen1+labelLen2+labelLen3+labelCount;
	}
	/* end of query... root TLD . reached.. */
	else
	{
		labelCount++;
		queryNameLen = labelLen1+labelLen2+labelLen3+labelLen4+labelCount;
	}

	/* retrieve the next label Len value for next octet */
	labelLen4 = qNameArg[queryNameLen];

	if(labelLen4 > 0)
	{
		/* update value of labelCount and queryNameLen */
		labelCount++;
		queryNameLen = labelLen1+labelLen2+labelLen3+labelLen4+labelCount;
	}
	/* end of query... root TLD . reached.. */
	else
	{
		labelCount++;
		queryNameLen = labelLen1+labelLen2+labelLen3+labelLen4+labelCount;
	}

    if (debug)
    {
    	printf("[-] Label Count: %u labelLen1>%u labelLen2>%u labelLen2>%u "
						"Total Len of Query> %u \n\n",
							labelCount,
								labelLen1,
									labelLen2,
										labelLen3,
    										queryNameLen);
    }
    // copy value into arg 2 for caller
    *qNameLen = queryNameLen;

	return 1;
}
