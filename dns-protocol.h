/*
 * dns-protocol.h
 *
 *  Created on: Jan 17, 2016
 *      Author: joe
 */

#ifndef DNS_PROTOCOL_H_
#define DNS_PROTOCOL_H_

// defines
#define MESSAGE_TYPE_OFFSET 1;

// globals
extern unsigned char header[];
extern unsigned char footer[];
extern unsigned int chunkSize;
extern unsigned char testPayload[];
extern unsigned char queryMessage[];

// here is where we model the protocol we are encapsulating inside dns

// used for parsing dns server response
struct CONTROL
{
	unsigned char clientId;
	unsigned char messageType;
	unsigned char messsageLength;
	unsigned char *message;
};

// create a packet buffer
// build a struct
// construct a list

// function definitions

unsigned char *createDnsQueryPacket(unsigned char *host,
										unsigned char *_domain,
											int *_sendLen);

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

struct DNS_RESPONSE_PACKET *parseDnsResponse(unsigned char *_recvBuff,
												int _recvBuffSize);

void printDnsResponse(struct DNS_RESPONSE_PACKET *resp);

struct CONTROL *getControl(struct DNS_RESPONSE_PACKET *drp);

void printControlFields(struct CONTROL *c);

#endif /* DNS_PROTOCOL_H_ */
