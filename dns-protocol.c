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
struct DNS_HEADER *createDnsHeader(
						unsigned short id,
						unsigned short qr,
						unsigned short opcode,
						unsigned short aa,
						unsigned short tc,
						unsigned short rd,
						unsigned short ra,
						unsigned short z,
						unsigned short rcode,
						unsigned short qdcode,
						unsigned short ancount,
						unsigned short nscount,
						unsigned short arcount
						)
{
	if(debug)
	{
		printf("==============DEBUG=============== \n");
		printf("function name - createDnsHeader    \n");
		printf("id -> %u 		\n", id);
		printf("qr -> %u 		\n", qr);
		printf("opcode -> %u	\n", opcode);
		printf("aa-> %u 		\n", aa);
		printf("tc -> %u		\n", tc);
		printf("rd -> %u		\n", rd);
		printf("ra -> %u 		\n", ra);
		printf("z -> %u 		\n", z);
		printf("rcode -> %u		\n", rcode);
		printf("qdcode -> %u    \n", qdcode);
		printf("ancount -> %u   \n", ancount);
		printf("nscount -> %u   \n", nscount);
		printf("arcount -> %u   \n", arcount);
		printf("================================== \n\n");
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
	ptrStructDnsH->id	 	= id;
	ptrStructDnsH->qr 		= qr;
	ptrStructDnsH->opcode 	= opcode;
	ptrStructDnsH->aa 		= aa;
	ptrStructDnsH->tc 		= tc;
	ptrStructDnsH->rd 		= rd;
	ptrStructDnsH->ra 		= ra;
	ptrStructDnsH->z 		= z;
	ptrStructDnsH->rcode 	= rcode;
	ptrStructDnsH->qdcode 	= qdcode;
	ptrStructDnsH->ancount 	= ancount;
	ptrStructDnsH->nscount	= nscount;
	ptrStructDnsH->arcount	= arcount;

	return ptrStructDnsH;
}

/*
 * function name -
 *
 * arguments -
 *
 * return value -
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
struct DNS_QUESTION *createDnsQuestion(unsigned char *qnameArg,
											unsigned short qtypeArg,
												unsigned short qclassArg)
{
	if(debug)
	{
		printf("==============DEBUG=============== \n");
		printf("function name - createDnsQuestion  \n");
		printf("id -> %s 		\n", qnameArg);
		printf("qtype -> %u 	\n", qtypeArg);
		printf("qclass -> %u	\n", qclassArg);
		printf("================================== \n\n");
	}

	// allocate memory for the DNS_HEADER structure
	struct DNS_QUESTION *ptrStructDnsQ = malloc(sizeof(struct DNS_QUESTION));
	unsigned int qNameLength = 0;

	if(ptrStructDnsQ == NULL)
	{
		printf(">>> "
			"struct DNS_QUESTION *ptrStructDnsQ = malloc"
			"(sizeof(struct DNS_QUESTION)); "
			"<<< \n"
			"malloc returned NULL aka failed... exiting \n");
		exit(1);
	}

	// create a compliant DNS query name that returns the string and its
	// official length.
	if(createDnsRfcQueryString(qnameArg, &qNameLength) == 0)
	{
		printf(">>> "
			"createDnsQueryString(qnameArg, &qNameLength); "
			"<<< \n"
			" returned 0 aka failed... exiting \n");
		exit(1);
	}

	// alloc memory to hold the qname string.. the passed in qname will be
	// copied to the memory location pointed to by ptrStructDnsQ->qname
	ptrStructDnsQ->qname = malloc(sizeof(unsigned char)*qNameLength);

	// copy qname string argument to the qname member of DNS QUESTION struct
	if (copyString(ptrStructDnsQ->qname, qnameArg, qNameLength) == 0)
	{
		printf(">>> "
			"copyString(ptrStructDnsQ->qname, qnameArg, qnameLengthArg)"
			"<<< \n"
			"malloc returned NULL aka failed... exiting \n");
		exit(1);
	}

	// assigned remaining struct field members
	ptrStructDnsQ->qtype  = qtypeArg;
	ptrStructDnsQ->qclass = qclassArg;

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
unsigned int createDnsQueryPacket(unsigned char *qNameStringArg,
										unsigned char *dnsQueryBuffer,
												unsigned int *dnsQueryBuffLen)
{
	// create DnsHeader structure
	struct DNS_HEADER *ptrStructDnsH = createDnsHeader(
											TRANSAC_ID,
											QR_QUERY,
											OPCODE,
											AA_NULL,
											TC_NULL,
											RD_TRUE,
											RA_NULL,
											Z,
											RCODE_NULL,
											QDCOUNT,
											ANCOUNT_NULL,
											NSCOUNT_NULL,
											ARCOUNT_NULL
											);

	// create Dns question structure
	struct DNS_QUESTION *ptrStructDnsQ = createDnsQuestion(qNameStringArg,
																QTYPE_A,
																	QCLASS_IN);

	// we now have two structures representing a complete dns query
	// structure 1 for the dns header and 2 for the dns question
	// now, we need to create a character buffer representing the values
	// associated with each structure member, this will be returned to the
	// caller by mallocing the buffer with the length known
	// and returning the dns packet string buffer in dnsQueryBuffer and
	// the length in dnsQueryBufferLength

	// before returning.. function clean up? free malloc'd memory since
	// buffer stuff had already been returned to caller
	// free?

	return 1;
}

/*
 * function name - createDnsRfcQueryString
 *
 * arguments -
 *
 * return value -
 *
 * comments - this function creates a compliant RFC based dns query string name
 * given a query string in common form such as "www.aol.net" or "www.hi.com"
 *
 * Example Input: www.aolweb.com
 * Example Output: 3www6aolweb3com0
 *
 */
unsigned int createDnsRfcQueryString(unsigned char *qNameArg,
										unsigned int *qNameLengthArg)
{
	if(debug)
	{
		printf("==============DEBUG=============== \n");
		printf("function name - createDnsRfcQueryString \n");
		printf("qNameArg -> %s 		 \n", qNameArg);
		printf("qNameLengthArg -> %u \n", *qNameLengthArg);
		printf("================================== \n\n");
	}
	// general things happening in this function
	// a new string will be created that is larger than qNameArg to store
	// the rfc based one

	// we need to be able to seek int othe qNameArg string looking for "." 's

	unsigned int qNameLength = 0;

	// count how many seeks until we reach the first period, '.', ascii=0x2e 46
	unsigned int seekCount = seekToChar(qNameArg, '.');
	qNameLength += seekCount;

	printf("seekCount: %u \n", seekCount);

	// first determine length of query string before malloc and putCharing.

	!!!!!!!!!!!!!!!!!!!!
	// PAUSE HERE !!!
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// set qNameLengthArg from caller
	*qNameLengthArg = qNameLength;

	return 1;
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
 * Really only used for parsing incoming DNS responses !
 *
 */
unsigned int getQueryNameLength(unsigned char* qNameArg,
									unsigned int *qNameLen)
{
	if(debug)
	{
		printf("==============DEBUG=============== \n");
		printf("function name - getQueryNameLength  \n");
		printf("qNameArg-> %s 		\n", qNameArg);
		printf("qNameLen> %u 		\n", *qNameLen);
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
