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
struct DNS_HEADER *createDnsHeader();

struct DNS_QUESTION *createDnsQuestion(unsigned char *qname,
											unsigned int *qNameLengthArg);

unsigned char *createDnsQueryPacket(unsigned char *qNameString,
										unsigned int *dnsQueryBuffLen);

unsigned char *createDnsRfcQueryString(unsigned char *qnameArg,
											unsigned int *rfcQNameLength);

#endif /* DNS_PROTOCOL_H_ */
