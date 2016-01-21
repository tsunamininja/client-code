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
 * function name -
 *
 * arguments -
 *
 * return value -
 *
 *
 */
struct DNS_HEADER *createDnsHeader()
{

	if(debug)
	{
		printf("===== createDnsHeader() ===== \n");
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
	ptrStructDnsH->id	 	= htons(0x1234); 		 // 2 bytes
	ptrStructDnsH->flags	= htons(QUERYFLAGS);	 // 2 bytes
	ptrStructDnsH->qdcode 	= htons(QDCOUNT);   	 // 1 bit
	ptrStructDnsH->ancount 	= ANCOUNT_NULL;  // 1 bit
	ptrStructDnsH->nscount	= NSCOUNT_NULL;  // 1 bit
	ptrStructDnsH->arcount	= ARCOUNT_NULL;  // 1 bit

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
											unsigned int *qNameLengthArg)
{
	if(debug)
	{
		printf("===== createDnsQuestion() ===== \n");
		printf("qNameArg -> %s 			\n", qNameArg);
		printf("qNameLengthArg -> %u 	\n", *qNameLengthArg);
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
	unsigned int sizeMalloc_dnsQname = sizeof(unsigned char)*rfcQNameLength;
    ptrStructDnsQ->qname = malloc(sizeMalloc_dnsQname);

	// copy qname string argument to the qname member of DNS QUESTION struct
	if (copyString(ptrStructDnsQ->qname,
						sizeMalloc_dnsQname,
							rfcQName,
								rfcQNameLength) == 0)
	{
		printf(">>> "
			"copyString(ptrStructDnsQ->qname, qnameArg, qnameLengthArg)"
			"<<< \n"
			"malloc returned NULL aka failed... exiting \n");
		exit(1);
	}

	// assigned remaining struct field members
	ptrStructDnsQ->qtype  = htons(QTYPE_A);
	ptrStructDnsQ->qclass = htons(QCLASS_IN);

	// return rfcQNameLength to caller
	*qNameLengthArg = rfcQNameLength;

	if(debug)
		printf("=====/end createDnsQuestion() ===== \n");

	return ptrStructDnsQ;
}


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
unsigned char *createDnsQueryPacket(unsigned char *qNameStringArg,
											unsigned int *dnsQueryBuffLen)
{
	// qNameLength will be added to size calculations for bufer length
	unsigned int qNameLength = 0;
	unsigned char *dnsQueryBuffer;
	unsigned int bbPos = 0;
	unsigned int sizednsh = sizeof(struct DNS_HEADER);
	unsigned int sizednsq = sizeof(struct DNS_QUESTION) - 4; // minus char*
	unsigned int mallocSize_dnsQueryBuffer;


	// create dns header structure
	struct DNS_HEADER *ptrStructDnsH = createDnsHeader();

	// create dns question structure
	struct DNS_QUESTION *ptrStructDnsQ = createDnsQuestion(qNameStringArg,
																&qNameLength);

	// ready to malloc socket buffer with qNameSize now known.
	//printf("qNameLength: %u \n", qNameLength);
	mallocSize_dnsQueryBuffer = sizednsh + sizednsq + qNameLength;
	dnsQueryBuffer = malloc(mallocSize_dnsQueryBuffer);

	// malloc failing here?

	printf("mallosize dns query buffer: %u \n", mallocSize_dnsQueryBuffer);
	printf("qNameLength: %u \n", qNameLength);

	// now align struct field packet members to buffer
	putShort(dnsQueryBuffer, &(ptrStructDnsH->id), 		&bbPos);
	putShort(dnsQueryBuffer, &(ptrStructDnsH->flags), 	&bbPos);
	putShort(dnsQueryBuffer, &(ptrStructDnsH->qdcode),  &bbPos);
	putShort(dnsQueryBuffer, &(ptrStructDnsH->ancount), &bbPos);
	putShort(dnsQueryBuffer, &(ptrStructDnsH->nscount), &bbPos);
	putShort(dnsQueryBuffer, &(ptrStructDnsH->arcount), &bbPos);

	// assign the dns question structure fields to dnsQueryBuffer array
	putString(dnsQueryBuffer,
				mallocSize_dnsQueryBuffer,
					ptrStructDnsQ->qname,
						qNameLength,
							&bbPos);

	putShort(dnsQueryBuffer, &(ptrStructDnsQ->qtype),  &bbPos);
	putShort(dnsQueryBuffer, &(ptrStructDnsQ->qclass), &bbPos);



	// before returning.. function clean up? free malloc'd memory since
	// buffer stuff had already been returned to caller
	// free?

	// we need to return caller dnsQUeryBufferLen.
	*dnsQueryBuffLen = bbPos;

	return dnsQueryBuffer;
}

/*
 * function name - createDnsRfcQueryString
 *
 * arguments -
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
