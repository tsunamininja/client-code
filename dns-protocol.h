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

// create a packet buffer
// build a struct
// construct a list

// function definitions

unsigned char *createDnsQueryPacket(unsigned char *_fqdn, int *sendLen);

struct DNS_HEADER *buildDnsHeader (
						unsigned short id,
						unsigned short flags,
						unsigned short qdcode,
						unsigned short ancount,
						unsigned short nscount,
						unsigned short arcount);

struct DNS_QUESTION *buildDnsQuestion(unsigned char *qNameArg,
											unsigned short qtypeArg,
												unsigned short qclassArg);

unsigned char *createDnsRfcQueryString(unsigned char *qnameArg,
											unsigned int *rfcQNameLength);

struct FQDN_NODE *constructFqdnList(unsigned char *_stdoutChunk,
									unsigned char *_domain,
										int _stdoutSize);

void printList(struct FQDN_NODE *head);

void push(struct FQDN_NODE *ptr_head,
				unsigned char *_fqdn,
					unsigned int _dataSize);

struct DNS_RESPONSE_PACKET *parseDnsResponsePacket(unsigned char *_recvBuff,
														int _recvBuffSize);

void printDnsResponse(struct DNS_RESPONSE_PACKET *resp);

#endif /* DNS_PROTOCOL_H_ */
