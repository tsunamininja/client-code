/*
 * dns-protocol.h
 *
 *  Created on: Jan 17, 2016
 *      Author: joe
 */

#ifndef DNS_PROTOCOL_H_
#define DNS_PROTOCOL_H_

// globals
extern unsigned char header[];
extern unsigned char footer[];
extern unsigned int chunkSize;
extern unsigned char testPayload[];
extern unsigned char queryMessage[];

// function definitions

int createDnsQueryPacket(unsigned char **sockBuff, unsigned char *userQName);

struct DNS_HEADER *createDnsHeader (
						unsigned short id,
						unsigned short flags,
						unsigned short qdcode,
						unsigned short ancount,
						unsigned short nscount,
						unsigned short arcount);

struct DNS_QUESTION *createDnsQuestion(unsigned char *qNameArg,
											unsigned short qtypeArg,
												unsigned short qclassArg);



unsigned char *createDnsRfcQueryString(unsigned char *qnameArg,
											unsigned int *rfcQNameLength);

#endif /* DNS_PROTOCOL_H_ */
