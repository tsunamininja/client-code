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
#include "dns-packet.h"   // structure definitions and size calculations
#include "/home/joe/workbench/dns-client/bufferHelper.h" // for copyString


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
 *
 */
struct DNS_QUESTION *createDnsQuestion(unsigned char *qnameArg,
											unsigned int qnameLengthArg,
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

	if(ptrStructDnsQ == NULL)
	{
		printf(">>> "
			"struct DNS_QUESTION *ptrStructDnsQ = malloc"
			"(sizeof(struct DNS_QUESTION)); "
			"<<< \n"
			"malloc returned NULL aka failed... exiting \n");
		exit(1);
	}

	// alloc memory to hold the qname string.. the passed in qname will be
	// copied to the memory location pointed to by ptrStructDnsQ->qname
	ptrStructDnsQ->qname = malloc(sizeof(unsigned char)*qnameLengthArg);

	// copy qname string argument to the qname member of DNS QUESTION struct
	if (copyString(ptrStructDnsQ->qname, qnameArg, qnameLengthArg) == 0)
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
