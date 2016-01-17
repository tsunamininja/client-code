/*
 * dns-protocol.h
 *
 *  Created on: Jan 17, 2016
 *      Author: joe
 */

#ifndef DNS_PROTOCOL_H_
#define DNS_PROTOCOL_H_


// global variables
extern unsigned int debug;

// function definitions
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
						);

struct DNS_QUESTION *createDnsQuestion(unsigned char *qname,
											unsigned int qnameLengthArg,
												unsigned short qtype,
													unsigned short qclass);


#endif /* DNS_PROTOCOL_H_ */
